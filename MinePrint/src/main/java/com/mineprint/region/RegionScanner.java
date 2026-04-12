package com.mineprint.region;

import net.minecraft.core.BlockPos;
import net.minecraft.core.Direction;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.block.SlabBlock;
import net.minecraft.world.level.block.StairBlock;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.block.state.properties.Half;
import net.minecraft.world.level.block.state.properties.SlabType;

import java.util.ArrayList;
import java.util.List;

public class RegionScanner {

    public static List<BlockEntry> scan(Level world, BlockRegion region) {
        List<BlockEntry> results = new ArrayList<>();

        for (int y = region.min.getY(); y <= region.max.getY(); y++) {
            for (int x = region.min.getX(); x <= region.max.getX(); x++) {
                for (int z = region.min.getZ(); z <= region.max.getZ(); z++) {

                    BlockPos current = new BlockPos(x, y, z);
                    BlockState state = world.getBlockState(current);

                    // Get block ID
                    String blockId = BuiltInRegistries.BLOCK
                            .getKey(state.getBlock())
                            .toString();

                    // Build metadata
                    BlockMetadata meta = new BlockMetadata();

                    // Face openness
                    Direction[] directions = Direction.values();
                    for (int i = 0; i < directions.length; i++) {
                        BlockPos neighbor = current.relative(directions[i]);
                        if (!region.contains(neighbor)) {
                            meta.openFaces[i] = true;
                        } else {
                            meta.openFaces[i] = world.getBlockState(neighbor).isAir();
                        }
                    }

                    // Shape detection
                    if (state.getBlock() instanceof StairBlock) {
                        meta.shapeType = BlockMetadata.ShapeType.STAIR;
                        Direction facing = state.getValue(StairBlock.FACING);
                        meta.stairFacing = facing.getName();
                        Half half = state.getValue(StairBlock.HALF);
                        meta.stairHalf = half.getSerializedName();

                    } else if (state.getBlock() instanceof SlabBlock) {
                        SlabType slabType = state.getValue(SlabBlock.TYPE);
                        meta.shapeType = switch (slabType) {
                            case BOTTOM -> BlockMetadata.ShapeType.SLAB_BOTTOM;
                            case TOP    -> BlockMetadata.ShapeType.SLAB_TOP;
                            case DOUBLE -> BlockMetadata.ShapeType.SLAB_DOUBLE;
                        };
                    }

                    // Normalize and store
                    BlockPos normalized = region.normalize(current);
                    results.add(new BlockEntry(normalized, blockId, meta));
                }
            }
        }

        return results;
    }
}