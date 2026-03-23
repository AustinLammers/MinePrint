package com.mineprint;

import com.mineprint.render.RegionBoxRenderer;
import net.fabricmc.api.ClientModInitializer;

public class MinePrintClient implements ClientModInitializer {
	@Override
	public void onInitializeClient() {
		RegionBoxRenderer.register();
	}
}