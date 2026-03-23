package com.mineprint.command;

import com.mineprint.item.CoordinateStick;
import com.mineprint.item.ModItems;
import com.mineprint.region.BlockEntry;
import com.mineprint.region.BlockRegion;
import com.mineprint.region.RegionScanner;
import com.mojang.brigadier.CommandDispatcher;
import com.mojang.brigadier.arguments.StringArgumentType;
import net.fabricmc.fabric.api.command.v2.CommandRegistrationCallback;
import net.minecraft.commands.CommandSourceStack;
import net.minecraft.commands.Commands;
import net.minecraft.core.BlockPos;
import net.minecraft.network.chat.Component;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.level.Level;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.List;

public class ToSTLCommand {

    private static final Path CONFIG_FILE = Path.of("mineprint_config.txt");
    private static String defaultSavePath = "mineprint_export.csv";

    public static void register() {
        loadDefaultPath();

        CommandRegistrationCallback.EVENT.register((dispatcher, registryAccess, environment) -> {
            registerCommands(dispatcher);
        });
    }

    private static void registerCommands(CommandDispatcher<CommandSourceStack> dispatcher) {

        // /to_csv | use default path
        // /to_csv <filepath> | use specified path
        dispatcher.register(Commands.literal("to_csv")
                .executes(context -> exportCsv(context.getSource(), defaultSavePath))
                .then(Commands.argument("filepath", StringArgumentType.greedyString())
                        .executes(context -> exportCsv(
                                context.getSource(),
                                fixPath(StringArgumentType.getString(context, "filepath"))
                        ))
                )
        );

        // /mineprint_setpath <filepath> | set and persist default path
        dispatcher.register(Commands.literal("mineprint_setpath")
                .then(Commands.argument("filepath", StringArgumentType.greedyString())
                        .executes(context -> {
                            String newPath = fixPath(StringArgumentType.getString(context, "filepath"));
                            defaultSavePath = newPath;
                            saveDefaultPath(newPath);
                            context.getSource().sendSuccess(() -> Component.literal(
                                    "Default save path set to: " + newPath), false);
                            return 1;
                        })
                )
        );
    }

    private static int exportCsv(CommandSourceStack source, String savePath) {

        // must be run by a player
        Player player;
        try {
            player = source.getPlayerOrException();
        } catch (Exception e) {
            source.sendFailure(Component.literal("Must be run by a player!"));
            return 0;
        }

        // check the player is holding the coord stick
        ItemStack main = player.getMainHandItem();
        ItemStack off  = player.getOffhandItem();
        ItemStack stack = main.getItem() == ModItems.COORD_STICK ? main
                : off.getItem()  == ModItems.COORD_STICK ? off
                : null;

        if (stack == null) {
            source.sendFailure(Component.literal("Hold the Coordinate Stick to use this command!"));
            return 0;
        }

        // check both positions are set
        if (!CoordinateStick.bothPosSet(stack)) {
            source.sendFailure(Component.literal("Set both positions with the Coordinate Stick first!"));
            return 0;
        }

        // build region and scan all blocks y->x->z
        BlockPos pos1 = CoordinateStick.getPos1(stack);
        BlockPos pos2 = CoordinateStick.getPos2(stack);
        BlockRegion region = new BlockRegion(pos1, pos2);
        Level world = player.level();

        source.sendSuccess(() -> Component.literal(
                "Scanning " + region.getTotalBlocks() + " blocks..."), false);

        List<BlockEntry> entries = RegionScanner.scan(world, region);

        // write CSV
        Path outputPath = Path.of(savePath);

        try {
            if (outputPath.getParent() != null) {
                Files.createDirectories(outputPath.getParent());
            }
        } catch (IOException e) {
            source.sendFailure(Component.literal("Failed to create directories: " + e.getMessage()));
            return 0;
        }

        try (BufferedWriter writer = Files.newBufferedWriter(outputPath,
                StandardOpenOption.CREATE,
                StandardOpenOption.TRUNCATE_EXISTING)) {

            writer.write("blockId,x,y,z,faces");
            writer.newLine();

            for (BlockEntry entry : entries) {
                writer.write(entry.toCsvRow());
                writer.newLine();
            }

        } catch (IOException e) {
            source.sendFailure(Component.literal("Failed to write file: " + e.getMessage()));
            return 0;
        }

        source.sendSuccess(() -> Component.literal(
                "Exported " + entries.size() + " blocks to: " + savePath), false);

        return 1;
    }

    // File Path Helpers

    private static String fixPath(String path) {
        // Restore Windows drive letter colon (e.g. "C/" → "C:/")
        if (path.length() >= 2 && Character.isLetter(path.charAt(0)) && path.charAt(1) == '/') {
            path = path.charAt(0) + ":/" + path.substring(2);
        }
        return path;
    }

    // Config Setting Persistence

    private static void loadDefaultPath() {
        try {
            if (Files.exists(CONFIG_FILE)) {
                String saved = fixPath(Files.readString(CONFIG_FILE).trim());
                if (!saved.isEmpty()) {
                    defaultSavePath = saved;
                }
            }
        } catch (IOException e) {
            // keep default if file can't be read
        }
    }

    private static void saveDefaultPath(String path) {
        try {
            Files.writeString(CONFIG_FILE, path,
                    StandardOpenOption.CREATE,
                    StandardOpenOption.TRUNCATE_EXISTING);
        } catch (IOException e) {
            // silently fail
        }
    }
}