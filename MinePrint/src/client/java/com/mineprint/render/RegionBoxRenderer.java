package com.mineprint.render;

import com.mineprint.item.CoordinateStick;
import com.mineprint.item.ModItems;
import com.mineprint.region.BlockRegion;
import com.mojang.blaze3d.vertex.PoseStack;
import com.mojang.blaze3d.vertex.VertexConsumer;
import net.fabricmc.fabric.api.client.rendering.v1.world.WorldRenderContext;
import net.fabricmc.fabric.api.client.rendering.v1.world.WorldRenderEvents;
import net.minecraft.client.Minecraft;
import net.minecraft.client.renderer.MultiBufferSource;
import net.minecraft.client.renderer.rendertype.RenderTypes;
import net.minecraft.core.BlockPos;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.ItemStack;
import org.joml.Matrix4f;

public class RegionBoxRenderer {

    public static void register() {
        WorldRenderEvents.AFTER_ENTITIES.register(RegionBoxRenderer::render);
    }

    private static void render(WorldRenderContext context) {
        Minecraft mc = Minecraft.getInstance();
        Player player = mc.player;
        if (player == null) return;

        ItemStack main = player.getMainHandItem();
        ItemStack off  = player.getOffhandItem();
        ItemStack stack = main.getItem() == ModItems.COORD_STICK ? main
                : off.getItem()  == ModItems.COORD_STICK ? off
                : null;

        if (stack == null) return;

        BlockPos pos1 = CoordinateStick.getPos1(stack);
        BlockPos pos2 = CoordinateStick.getPos2(stack);
        if (pos1 == null || pos2 == null) return;

        BlockRegion region = new BlockRegion(pos1, pos2);

        PoseStack poseStack = context.matrices();
        poseStack.pushPose();
        var camera = mc.gameRenderer.getMainCamera().position();
        poseStack.translate(-camera.x, -camera.y, -camera.z);

        MultiBufferSource.BufferSource buffer = mc.renderBuffers().bufferSource();
        VertexConsumer lines = buffer.getBuffer(RenderTypes.lines());

        drawBox(poseStack, lines, region);

        buffer.endBatch(RenderTypes.lines());
        poseStack.popPose();
    }

    private static void drawBox(PoseStack poseStack, VertexConsumer lines, BlockRegion region) {
        Matrix4f mat = poseStack.last().pose();

        float x1 = region.min.getX();
        float y1 = region.min.getY();
        float z1 = region.min.getZ();
        float x2 = region.max.getX() + 1f;
        float y2 = region.max.getY() + 1f;
        float z2 = region.max.getZ() + 1f;

        float r = 0.0f, g = 0.7f, b = 1.0f, a = 1.0f;

        // Bottom face
        line(lines, mat, x1,y1,z1, x2,y1,z1, r,g,b,a);
        line(lines, mat, x2,y1,z1, x2,y1,z2, r,g,b,a);
        line(lines, mat, x2,y1,z2, x1,y1,z2, r,g,b,a);
        line(lines, mat, x1,y1,z2, x1,y1,z1, r,g,b,a);

        // Top face
        line(lines, mat, x1,y2,z1, x2,y2,z1, r,g,b,a);
        line(lines, mat, x2,y2,z1, x2,y2,z2, r,g,b,a);
        line(lines, mat, x2,y2,z2, x1,y2,z2, r,g,b,a);
        line(lines, mat, x1,y2,z2, x1,y2,z1, r,g,b,a);

        // Vertical edges
        line(lines, mat, x1,y1,z1, x1,y2,z1, r,g,b,a);
        line(lines, mat, x2,y1,z1, x2,y2,z1, r,g,b,a);
        line(lines, mat, x2,y1,z2, x2,y2,z2, r,g,b,a);
        line(lines, mat, x1,y1,z2, x1,y2,z2, r,g,b,a);
    }

    private static void line(VertexConsumer lines, Matrix4f mat,
                             float x1, float y1, float z1,
                             float x2, float y2, float z2,
                             float r, float g, float b, float a) {
        lines.addVertex(mat, x1, y1, z1).setColor(r, g, b, a).setNormal(1, 0, 0).setLineWidth(10.0f);
        lines.addVertex(mat, x2, y2, z2).setColor(r, g, b, a).setNormal(1, 0, 0).setLineWidth(10.0f);
    }
}