package com.mineprint.region;

import net.minecraft.core.BlockPos;

public class BlockRegion {

    public final BlockPos min;
    public final BlockPos max;

    public BlockRegion(BlockPos a, BlockPos b) {
        this.min = new BlockPos(
                Math.min(a.getX(), b.getX()),
                Math.min(a.getY(), b.getY()),
                Math.min(a.getZ(), b.getZ())
        );

        this.max = new BlockPos(
                Math.max(a.getX(), b.getX()),
                Math.max(a.getY(), b.getY()),
                Math.max(a.getZ(), b.getZ())
        );
    }

    public int getSizeX() { return max.getX() - min.getX() + 1; }
    public int getSizeY() { return max.getY() - min.getY() + 1; }
    public int getSizeZ() { return max.getZ() - min.getZ() + 1; }

    public int getTotalBlocks() {
        return (getSizeX() * getSizeY() * getSizeZ());
    }

    public BlockPos normalize(BlockPos worldPos) {
        return new BlockPos(
                worldPos.getX() - min.getX(),
                worldPos.getY() - min.getY(),
                worldPos.getZ() - min.getZ()
        );
    }

    public boolean contains(BlockPos pos) {
        return pos.getX() >= min.getX() && pos.getX() <= max.getX()
            && pos.getY() >= min.getY() && pos.getY() <= max.getY()
            && pos.getZ() >= min.getZ() && pos.getZ() <= max.getZ();
    }
}
