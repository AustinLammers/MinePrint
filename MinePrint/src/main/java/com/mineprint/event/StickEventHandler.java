package com.mineprint.event;

import com.mineprint.item.CoordinateStick;
import net.fabricmc.fabric.api.event.player.AttackBlockCallback;
import net.fabricmc.fabric.api.event.player.UseBlockCallback;
import net.minecraft.core.BlockPos;
import net.minecraft.network.chat.Component;
import net.minecraft.world.InteractionResult;
import net.minecraft.world.item.ItemStack;

public class StickEventHandler {
    public static void register() {

        UseBlockCallback.EVENT.register((player, world, hand, hitResult) -> {
            if (world.isClientSide()) return InteractionResult.PASS;
            ItemStack stack = player.getItemInHand(hand);

            if (!(stack.getItem() instanceof CoordinateStick)) {
                return InteractionResult.PASS;
            }

            BlockPos pos = hitResult.getBlockPos();
            CoordinateStick.setPos1(stack, pos);
            player.displayClientMessage(Component.literal(
                    "Pos1 set: " + pos.getX() + ", " + pos.getY() + ", " + pos.getZ()
            ), false);

            return InteractionResult.SUCCESS;
        });

        AttackBlockCallback.EVENT.register((player, world, hand, pos, direction) -> {
            if (world.isClientSide()) return InteractionResult.PASS;
            ItemStack stack = player.getItemInHand(hand);

            if (!(stack.getItem() instanceof CoordinateStick)) {
                return InteractionResult.PASS;
            }

            CoordinateStick.setPos2(stack, pos);
            player.displayClientMessage(Component.literal(
                    "Pos2 set: " + pos.getX() + ", " + pos.getY() + ", " + pos.getZ()
            ), false);

            return InteractionResult.SUCCESS;
        });
    }
}
