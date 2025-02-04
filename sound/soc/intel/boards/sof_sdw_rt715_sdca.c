// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2020 Intel Corporation

/*
 *  sof_sdw_rt715_sdca - Helpers to handle RT715-SDCA from generic machine driver
 */

#include <linaos/device.h>
#include <linaos/errno.h>
#include <sound/soc.h>
#include <sound/soc-acpi.h>
#include "sof_sdw_common.h"

static int rt715_sdca_rtd_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_card *card = rtd->card;

	card->components = devm_kasprintf(card->dev, GFP_KERNEL,
					  "%s mic:rt715-sdca",
					  card->components);
	if (!card->components)
		return -ENOMEM;

	return 0;
}

int sof_sdw_rt715_sdca_init(const struct snd_soc_acpi_link_adr *link,
			    struct snd_soc_dai_link *dai_links,
			    struct sof_sdw_codec_info *info,
			    bool playback)
{
	/*
	 * DAI ID is fixed at SDW_DMIC_DAI_ID for 715-SDCA to
	 * keep sdw DMIC and HDMI setting static in UCM
	 */
	if (sof_sdw_quirk & SOF_RT715_DAI_ID_FIX)
		dai_links->id = SDW_DMIC_DAI_ID;

	dai_links->init = rt715_sdca_rtd_init;

	return 0;
}
