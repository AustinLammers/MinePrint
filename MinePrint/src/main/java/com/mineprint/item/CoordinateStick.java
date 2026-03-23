package com.mineprint.item;

import net.minecraft.core.BlockPos;
import net.minecraft.core.component.DataComponents;
import net.minecraft.nbt.CompoundTag;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.component.CustomData;

public class CoordinateStick extends Item {
    public CoordinateStick(Properties properties) {
        super(properties);
    }

    public static void setPos1(ItemStack stack, BlockPos pos) {
        CompoundTag tag = getOrCreateTag(stack);
        tag.putInt("pos1_x", pos.getX());
        tag.putInt("pos1_y", pos.getY());
        tag.putInt("pos1_z", pos.getZ());
        tag.putBoolean("has_pos1", true);
        stack.set(DataComponents.CUSTOM_DATA, CustomData.of(tag));
    }

    public static void setPos2(ItemStack stack, BlockPos pos) {
        CompoundTag tag = getOrCreateTag(stack);
        tag.putInt("pos2_x", pos.getX());
        tag.putInt("pos2_y", pos.getY());
        tag.putInt("pos2_z", pos.getZ());
        tag.putBoolean("has_pos2", true);
        stack.set(DataComponents.CUSTOM_DATA, CustomData.of(tag));
    }

    public static BlockPos getPos1(ItemStack stack) {
        CompoundTag tag = getTag(stack);
        if (tag == null || !tag.getBoolean("has_pos1").orElse(false)) return null;  // ← orElse here
        return new BlockPos(tag.getInt("pos1_x").get(),
                tag.getInt("pos1_y").get(),
                tag.getInt("pos1_z").get());
    }

    public static BlockPos getPos2(ItemStack stack) {
        CompoundTag tag = getTag(stack);
        if (tag == null || !tag.getBoolean("has_pos2").orElse(false)) return null;  // ← orElse here
        return new BlockPos(tag.getInt("pos2_x").get(),
                tag.getInt("pos2_y").get(),
                tag.getInt("pos2_z").get());
    }

    public static boolean bothPosSet(ItemStack stack) {
        CompoundTag tag = getTag(stack);
        return tag != null
                && tag.getBoolean("has_pos1").orElse(false)   // ← orElse here
                && tag.getBoolean("has_pos2").orElse(false);  // ← orElse here
    }

    private static CompoundTag getOrCreateTag(ItemStack stack) {
        CustomData existing = stack.get(DataComponents.CUSTOM_DATA);
        return (existing != null ? existing.copyTag() : new CompoundTag());
    }

    private static CompoundTag getTag(ItemStack stack) {
        CustomData existing = stack.get(DataComponents.CUSTOM_DATA);
        return (existing != null ? existing.copyTag() :  null);
    }
}