package com.mineprint.region;

public class BlockMetadata {

    // one entry per face: North, South, East, West, Up, Down
    public boolean[] openFaces = new boolean[6];

    public static final int NORTH = 0;
    public static final int SOUTH = 1;
    public static final int EAST  = 2;
    public static final int WEST  = 3;
    public static final int UP    = 4;
    public static final int DOWN  = 5;

    public String serialize() {
        StringBuilder sb = new StringBuilder();
        String[] names = {"N", "S", "E", "W", "U", "D"};
        for (int i = 0; i < 6; i++) {
            if (i > 0) sb.append("|");
            sb.append(names[i]).append(":").append(openFaces[i] ? "1" : "0");
        }
        return sb.toString();
    }
}