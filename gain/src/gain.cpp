#include <cassert>
#include <clap/clap.h>
#include <clap/entry.h>
#include <clap/factory/plugin-factory.h>
#include <cmath>
#include <cstdlib>
#include <cstring>

struct GainData {
    const clap_host_t *host;
    double sample_rate;
    double gain;
};

static const clap_plugin_descriptor_t pluginDescriptor = {
	.clap_version = CLAP_VERSION_INIT,
	.id = "baba.Gain",
	.name = "Gain",
	.vendor = "baba",
	.url = "https://nakst.gitlab.io",
	.manual_url = "https://nakst.gitlab.io",
	.support_url = "https://nakst.gitlab.io",
	.version = "1.0.0",
	.description = "Gain plugin to test CLAP.",

	.features = (const char *[]) {
        CLAP_PLUGIN_FEATURE_UTILITY,
		CLAP_PLUGIN_FEATURE_STEREO,
		NULL,
	},
};



static const clap_plugin_t gain_plugin_def = {
    .desc = &pluginDescriptor,
    .plugin_data = nullptr,
    .init = [] (const clap_plugin *plugin) -> bool {
        return true;
    },
    .destroy = [] (const clap_plugin *plugin) {
        GainData *data = (GainData *) plugin->plugin_data;
        free(data);
    },
    .activate = [] (const clap_plugin *plugin, double sample_rate, uint32_t min_frames, uint32_t max_frames) -> bool {
        GainData *data = (GainData *) plugin->plugin_data;
        data->sample_rate = sample_rate;
        return true;
    },
    .deactivate = [] (const clap_plugin_t *plugin) {
    },
    .start_processing = [] (const clap_plugin_t *plugin) {
        return true;
    },
    .stop_processing = [] (const clap_plugin_t *plugin) {
    },
    .reset = [] (const clap_plugin *plugin) {

    },
    .process = [] (const clap_plugin *plugin, const clap_process_t *process) -> clap_process_status {
        GainData *data = (GainData*) plugin->plugin_data;

        assert(process->audio_outputs_count == 1);

        for(int frame = 0; frame < process->frames_count; frame++) {
            uint32_t inL = process->audio_inputs[0].data32[0][frame];
            uint32_t inR = process->audio_inputs[0].data32[1][frame];

            process->audio_outputs[0].data32[0][frame] = (double)inL * std::pow(10.0, data->gain/10.0);
            process->audio_outputs[0].data32[1][frame] = (double)inR * std::pow(10.0, data->gain/10.0);
        }

        return CLAP_PROCESS_CONTINUE;
    },
    .get_extension = [] (const clap_plugin *plugin, const char *id) -> const void * {
		// TODO.
		return nullptr;
	},
	.on_main_thread = [] (const clap_plugin *_plugin) {
	},
};

static const clap_plugin_factory_t pluginFactory = {
    .get_plugin_count = [] (const clap_plugin_factory *factory) -> uint32_t {
        return 1;
    },

    .get_plugin_descriptor = [] (const clap_plugin_factory *factory, uint32_t index) -> const clap_plugin_descriptor * {
        return index == 0 ? &pluginDescriptor : nullptr;
    },

    .create_plugin = [] (const clap_plugin_factory *factory, const clap_host_t *host, const char *pluginID) -> const clap_plugin_t * {
        if(!clap_version_is_compatible(host->clap_version)) {
            return nullptr;
        }

        GainData *gain_data = (GainData*) calloc(1, sizeof(GainData));
        gain_data->host = host;

        clap_plugin_t *gain_plugin = (clap_plugin_t*) malloc(sizeof(clap_plugin_t));
        memcpy(gain_plugin, &gain_plugin_def, sizeof(clap_plugin_t));
        gain_plugin->plugin_data = gain_data;

        return gain_plugin;
    }
};

extern "C" const clap_plugin_entry clap_entry = {
    .clap_version = CLAP_VERSION_INIT,
    .init = [] (const char *path) -> bool {
        return true;
    },
    .deinit = []() {},
    .get_factory = [] (const char *factoryID) -> const void * {
        return strcmp(factoryID, CLAP_PLUGIN_FACTORY_ID) ? nullptr : &pluginFactory;
    }
};
