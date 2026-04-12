package com.mineprint.region;

public class BlockMetadata {

    // Face openness
    public boolean[] openFaces = new boolean[6];
    public static final int NORTH = 0;
    public static final int SOUTH = 1;
    public static final int EAST  = 2;
    public static final int WEST  = 3;
    public static final int UP    = 4;
    public static final int DOWN  = 5;

    // Block shape type
    public enum ShapeType { FULL, SLAB_BOTTOM, SLAB_TOP, SLAB_DOUBLE, STAIR }
    public ShapeType shapeType = ShapeType.FULL;

    // Stair facing direction (only relevant if shapeType == STAIR)
    public String stairFacing = "none"; // north, south, east, west
    public String stairHalf = "none"; // bottom, top

    public String toDataString() {
        StringBuilder sb = new StringBuilder();
        String[] names = {"N", "S", "E", "W", "U", "D"};
        for (int i = 0; i < 6; i++) {
            if (i > 0) sb.append("|");
            sb.append(names[i]).append(":").append(openFaces[i] ? "1" : "0");
        }
        sb.append("|shape:").append(shapeType.name());
        sb.append("|facing:").append(stairFacing);
        sb.append("|half:").append(stairHalf);
        return sb.toString();
    }
}