// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2021 @KuuroZero
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (@KuuroZero)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct s6e8aa5x01_ams604rm01 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct s6e8aa5x01_ams604rm01 *to_s6e8aa5x01_ams604rm01(struct drm_panel *panel)
{
	return container_of(panel, struct s6e8aa5x01_ams604rm01, panel);
}

#define dsi_generic_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_generic_write(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void s6e8aa5x01_ams604rm01_reset(struct s6e8aa5x01_ams604rm01 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int s6e8aa5x01_ams604rm01_on(struct s6e8aa5x01_ams604rm01 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(20);

	dsi_generic_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xca,
			      0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x80,
			      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
			      0x00);
	dsi_generic_write_seq(dsi, 0xb2, 0x00, 0x10);
	dsi_generic_write_seq(dsi, 0xf7, 0x03);
	dsi_generic_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_generic_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xc0, 0xd8, 0xd8, 0x00);
	dsi_generic_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_generic_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xcc, 0x4c);
	dsi_generic_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_generic_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xe7, 0xed, 0xc7, 0x23, 0x67);
	dsi_generic_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_generic_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xf2, 0x0a, 0x0e, 0xb9, 0x01);
	dsi_generic_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	msleep(120);

	return 0;
}

static int s6e8aa5x01_ams604rm01_off(struct s6e8aa5x01_ams604rm01 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int s6e8aa5x01_ams604rm01_prepare(struct drm_panel *panel)
{
	struct s6e8aa5x01_ams604rm01 *ctx = to_s6e8aa5x01_ams604rm01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	s6e8aa5x01_ams604rm01_reset(ctx);

	ret = s6e8aa5x01_ams604rm01_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int s6e8aa5x01_ams604rm01_unprepare(struct drm_panel *panel)
{
	struct s6e8aa5x01_ams604rm01 *ctx = to_s6e8aa5x01_ams604rm01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = s6e8aa5x01_ams604rm01_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode s6e8aa5x01_ams604rm01_mode = {
	.clock = (720 + 172 + 12 + 20) * (1480 + 14 + 5 + 5) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 172,
	.hsync_end = 720 + 172 + 12,
	.htotal = 720 + 172 + 12 + 20,
	.vdisplay = 1480,
	.vsync_start = 1480 + 14,
	.vsync_end = 1480 + 14 + 5,
	.vtotal = 1480 + 14 + 5 + 5,
	.width_mm = 67,
	.height_mm = 138,
};

static int s6e8aa5x01_ams604rm01_get_modes(struct drm_panel *panel,
					   struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &s6e8aa5x01_ams604rm01_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs s6e8aa5x01_ams604rm01_panel_funcs = {
	.prepare = s6e8aa5x01_ams604rm01_prepare,
	.unprepare = s6e8aa5x01_ams604rm01_unprepare,
	.get_modes = s6e8aa5x01_ams604rm01_get_modes,
};

static int s6e8aa5x01_ams604rm01_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

// TODO: Check if /sys/class/backlight/.../actual_brightness actually returns
// correct values. If not, remove this function.
static int s6e8aa5x01_ams604rm01_bl_get_brightness(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_get_display_brightness(dsi, &brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return brightness;
}

static const struct backlight_ops s6e8aa5x01_ams604rm01_bl_ops = {
	.update_status = s6e8aa5x01_ams604rm01_bl_update_status,
	.get_brightness = s6e8aa5x01_ams604rm01_bl_get_brightness,
};

static struct backlight_device *
s6e8aa5x01_ams604rm01_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 365,
		.max_brightness = 365,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &s6e8aa5x01_ams604rm01_bl_ops, &props);
}

static int s6e8aa5x01_ams604rm01_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct s6e8aa5x01_ams604rm01 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST;

	drm_panel_init(&ctx->panel, dev, &s6e8aa5x01_ams604rm01_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->panel.backlight = s6e8aa5x01_ams604rm01_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		return ret;
	}

	return 0;
}

static int s6e8aa5x01_ams604rm01_remove(struct mipi_dsi_device *dsi)
{
	struct s6e8aa5x01_ams604rm01 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id s6e8aa5x01_ams604rm01_of_match[] = {
	{ .compatible = "mdss,s6e8aa5x01-ams604rm01" }, // @KuuroZero
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, s6e8aa5x01_ams604rm01_of_match);

static struct mipi_dsi_driver s6e8aa5x01_ams604rm01_driver = {
	.probe = s6e8aa5x01_ams604rm01_probe,
	.remove = s6e8aa5x01_ams604rm01_remove,
	.driver = {
		.name = "panel-s6e8aa5x01-ams604rm01",
		.of_match_table = s6e8aa5x01_ams604rm01_of_match,
	},
};
module_mipi_dsi_driver(s6e8aa5x01_ams604rm01_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // @KuuroZero
MODULE_DESCRIPTION("DRM driver for ss_dsi_panel_S6E8AA5X01_AMS604RM01_HD");
MODULE_LICENSE("GPL v2");
