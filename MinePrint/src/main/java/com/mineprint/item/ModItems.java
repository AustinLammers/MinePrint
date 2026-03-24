package com.mineprint.item;

import net.minecraft.core.Registry;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.resources.Identifier;
import net.minecraft.world.item.Item;

public final class ModItems {

    private ModItems() {}

    public static final CoordinateStick COORD_STICK = registerStick("coord_stick");

    private static CoordinateStick registerStick(String name) {
        Identifier location = Identifier.parse("mineprint:" + name);
        ResourceKey<Item> key = ResourceKey.create(Registries.ITEM, location);
        CoordinateStick item = new CoordinateStick(new Item.Properties().setId(key));
        Registry.register(BuiltInRegistries.ITEM, location, item);
        return item;
    }

    public static void initialize() {}
}