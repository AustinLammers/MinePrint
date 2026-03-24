package com.mineprint.region;

import net.minecraft.core.BlockPos;
import net.minecraft.core.Direction;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.block.state.BlockState;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Queue;
import java.util.Set;

public class RegionScanner {

    public static List<BlockEntry> scan(Level world, BlockRegion region) {
        List<BlockEntry> results = new ArrayList<>();
        Set<BlockPos> visited = new HashSet<>();
        Queue<BlockPos> queue = new ArrayDeque<>();

        // run a BFS starting from the minimum corner.
        queue.add(region.min);
        visited.add(region.min);

        while (!queue.isEmpty()) {
            BlockPos current = queue.poll();

            // get the blocks ID.
            BlockState state = world.getBlockState(current);
            String blockId = BuiltInRegistries.BLOCK
                    .getKey(state.getBlock())
                    .toString();

            // build our metadata by checking each block face to see if it's open.
            BlockMetadata meta = new BlockMetadata();
            Direction[] directions = Direction.values();
            for (int i = 0; i < directions.length; i++) {
                BlockPos neighbor = current.relative(directions[i]);
                if (!region.contains(neighbor)) {
                    meta.openFaces[i] = true;
                } else {
                    meta.openFaces[i] = world.getBlockState(neighbor).isAir();
                }
            }

            // normalize and store our data.
            BlockPos normalized = region.normalize(current);
            results.add(new BlockEntry(normalized, blockId, meta));

            // queue unvisited neighbors inside our region.
            for (Direction dir : Direction.values()) {
                BlockPos neighbor = current.relative(dir);
                if (region.contains(neighbor) && !visited.contains(neighbor)) {
                    visited.add(neighbor);
                    queue.add(neighbor);
                }
            }
        }

        return results;
    }
}