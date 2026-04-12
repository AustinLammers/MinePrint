package com.mineprint.region;

import com.jcraft.jorbis.Block;
import net.minecraft.core.BlockPos;

public class BlockEntry {

    public final BlockPos normalizedPos;
    public final String blockId;
    public final BlockMetadata metadata;

    public BlockEntry(BlockPos normalizedPos, String blockId, BlockMetadata metadata) {
        this.normalizedPos = normalizedPos;
        this.blockId = blockId;
        this.metadata = metadata;
    }

    public String toCsvRow() {
        return blockId + ","
             + normalizedPos.getX() + ","
             + normalizedPos.getY() + ","
             + normalizedPos.getZ() + ","
             + metadata.toDataString();
    }
}
