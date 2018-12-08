QT += widgets qt3support
CONFIG += link_pkgconfig
CONFIG += no_keywords


HEADERS += \
    cheesetracker/interface__QT/instrument_edit.h \
    cheesetracker/interface__QT/interface.h \
    cheesetracker/interface__QT/mdi_main_window.h \
    cheesetracker/interface__QT/order_and_defaults_editor.h \
    cheesetracker/interface__QT/orderlist_edit.h \
    cheesetracker/interface__QT/pattern_edit.h \
    cheesetracker/interface__QT/pattern_edit_widget.h \
    cheesetracker/interface__QT/sample_edit.h \
    cheesetracker/interface__QT/sample_instrument_table.h \
    cheesetracker/interface__QT/sample_player_fdialog.h \
    cheesetracker/interface__QT/settings_window.h \
    cheesetracker/interface__QT/table_base.h \
    cheesetracker/interface__QT/variables_edit.h \
    cheesetracker/interface_binds/editor.h \
    cheesetracker/interface_binds/interface_help_text.h \
    cheesetracker/interface_binds/player_rt_keyboard.h \
    cheesetracker/interface_binds/tracker_instance.h \
    cheesetracker/loaders/loader_ct.h \
    cheesetracker/loaders/loader_it.h \
    cheesetracker/loaders/loader_libaudiofile.h \
    cheesetracker/loaders/loader_raw.h \
    cheesetracker/loaders/loader_s3m.h \
    cheesetracker/loaders/loader_wav.h \
    cheesetracker/loaders/loader_xm.h \
    cheesetracker/savers/saver_ct.h \
    cheesetracker/savers/saver_it.h \
    cheesetracker/savers/saver_raw.h \
    cheesetracker/savers/saver_wav.h \
    cheesetracker/trackercore/channel_control.h \
    cheesetracker/trackercore/Error.h \
    cheesetracker/trackercore/file_format_manager.h \
    cheesetracker/trackercore/file_reader.h \
    cheesetracker/trackercore/file_writer.h \
    cheesetracker/trackercore/freq_conversion.h \
    cheesetracker/trackercore/instrument.h \
    cheesetracker/trackercore/loader.h \
    cheesetracker/trackercore/note.h \
    cheesetracker/trackercore/order.h \
    cheesetracker/trackercore/pattern.h \
    cheesetracker/trackercore/player_data.h \
    cheesetracker/trackercore/range_defs.h \
    cheesetracker/trackercore/sample.h \
    cheesetracker/trackercore/sample_data_deprecated.h \
    cheesetracker/trackercore/saver.h \
    cheesetracker/trackercore/song.h \
    cheesetracker/trackercore/tracker_instance.h \
    cheesetracker/trackercore/tracker_voice.h \
    cheesetracker/trackercore/voice_control.h \
    common/components/audio/dds_helpers.h \
    common/components/audio/edit_effect.h \
    common/components/audio/effect.h \
    common/components/audio/effect_chain.h \
    common/components/audio/effect_parameter_chain.h \
    common/components/audio/effect_source.h \
    common/components/audio/effect_source_internal.h \
    common/components/audio/effect_source_manager.h \
    common/components/audio/envelope.h \
    common/components/audio/iir_svf.h \
    common/components/audio/lfo.h \
    common/components/audio/midi_voice.h \
    common/components/audio/mixer.h \
    common/components/audio/mixer_base.h \
    common/components/audio/resampler.h \
    common/components/audio/resampler_instance.h \
    common/components/audio/resampler_manager.h \
    common/components/audio/resampler_new.h \
    common/components/audio/ring_buffer.h \
    common/components/audio/sample_conversion.h \
    common/components/audio/sample_data.h \
    common/components/audio/sample_data_fixedpoint.h \
    common/components/audio/sound_driver.h \
    common/components/audio/sound_driver_dummy.h \
    common/components/audio/sound_driver_manager.h \
    common/components/audio/tables.h \
    common/components/audio/voice.h \
    common/components/data/config_handler.h \
    common/components/data/dds.h \
    common/components/data/dds_packer.h \
    common/components/data/fixedpoint.h \
    common/components/data/keyboard_input.h \
    common/components/data/property_bridges.h \
    common/components/data/selection.h \
    common/components/file/file_access.h \
    common/components/file/file_browser.h \
    common/components/midi/engine.h \
    common/components/midi/engine_manager.h \
    common/components/midi/midi_client.h \
    common/components/midi/midi_client_manager.h \
    common/components/midi/midi_control.h \
    common/components/midi/midi_control_base.h \
    common/components/midi/midi_controls.h \
    common/components/midi/midi_event.h \
    common/components/midi/midi_out_device.h \
    common/components/midi/midioutdevicemanager.h \
    common/defines/cycles.h \
    common/defines/fixedpoint_defs.h \
    common/defines/hash_stuff.h \
    common/defines/interface_defs.h \
    common/defines/midi_defs.h \
    common/defines/prealloc_list.h \
    common/defines/pvector.h \
    common/defines/sample_defs.h \
    common/defines/string_hash.h \
    common/defines/typedefs.h \
    common/drivers/posix/midi_client_alsa.h \
    common/drivers/posix/midi_out_device_alsa.h \
    common/drivers/posix/midi_out_device_oss.h \
    common/drivers/posix/mutex_lock_pthreads.h \
    common/drivers/posix/sound_driver_jack.h \
    common/drivers/posix/sound_driver_oss.h \
    common/drivers/posix/tcp_client_posix.h \
    common/drivers/posix/tcp_server_posix.h \
    common/drivers/posix/timer_rtc.h \
    common/drivers/posix/timer_sigalarm.h \
    common/drivers/rtaudio/rt_audio.h \
    common/drivers/rtaudio/sound_driver_rtaudio.h \
    common/drivers/win32/midi_out_device_win32.h \
    common/drivers/win32/sound_driver_win32.h \
    common/drivers/win32/sound_driver_winmm.h \
    common/drivers/win32/timer_winthreads.h \
    common/interface__QT/audio/audio_config.h \
    common/interface__QT/audio/effect_buffers_editor.h \
    common/interface__QT/audio/effect_chain_editor.h \
    common/interface__QT/audio/envelope_editor.h \
    common/interface__QT/audio/envelope_point_editor.h \
    common/interface__QT/audio/mixer_effects_manager.h \
    common/interface__QT/audio/note_bridge.h \
    common/interface__QT/audio/resampler_config.h \
    common/interface__QT/audio/sample_editor.h \
    common/interface__QT/audio/sample_editor_clipboard.h \
    common/interface__QT/audio/sample_editor_effects.h \
    common/interface__QT/audio/sample_editor_format.h \
    common/interface__QT/audio/sample_viewer.h \
    common/interface__QT/audio/sample_viewer_zoom.h \
    common/interface__QT/helpers/ccolor_bridge.h \
    common/interface__QT/helpers/ccolor_list.h \
    common/interface__QT/helpers/ccolor_panel.h \
    common/interface__QT/helpers/clist_manager.h \
    common/interface__QT/helpers/cspinbutton.h \
    common/interface__QT/helpers/font_bridge.h \
    common/interface__QT/helpers/keyboard_input_config.h \
    common/interface__QT/helpers/property_bridge_edit.h \
    common/interface__QT/helpers/property_bridge_edit_list.h \
    common/interface__QT/helpers/vertical_scrolled_window.h \
    common/interface__QT/helpers/vlabel.h \
    common/interface__QT/popups/cspindialog.h \
    common/interface__QT/popups/effect_select_popup.h \
    common/interface__QT/popups/text_area_popup.h \
    common/os/multireader_lock.h \
    common/os/mutex_lock.h \
    common/os/mutex_queue.h \
    common/os/path_data.h \
    common/os/tcp_client.h \
    common/os/tcp_server.h \
    common/os/threaded_class.h \
    common/os/timer.h \
    common/plugins/edit_effects/simple_edit_effects.h \
    common/plugins/effects/custom/chorus.h \
    common/plugins/effects/custom/chorus_effect.h \
    common/plugins/effects/custom/custom_fx_param_list.h \
    common/plugins/effects/custom/dummy_effect.h \
    common/plugins/effects/custom/effect_amp.h \
    common/plugins/effects/custom/effect_distort.h \
    common/plugins/effects/custom/effect_distortion.h \
    common/plugins/effects/custom/effect_echo.h \
    common/plugins/effects/custom/effect_equalizer.h \
    common/plugins/effects/custom/effect_pshift.h \
    common/plugins/effects/custom/effect_stereo_enhancer.h \
    common/plugins/effects/custom/effect_stereo_fx.h \
    common/plugins/effects/custom/freeverb_effect.h \
    common/plugins/effects/freeverb/allpass.h \
    common/plugins/effects/freeverb/comb.h \
    common/plugins/effects/freeverb/denormals.h \
    common/plugins/effects/freeverb/revmodel.h \
    common/plugins/effects/freeverb/tuning.h \
    common/plugins/effects/ladspa/ladspa.h \
    common/plugins/effects/ladspa/ladspa_effect.h \
    common/plugins/effects/ladspa/ladspa_effect_source.h \
    common/plugins/resamplers/helpers.h \
    common/plugins/resamplers/resampler_cosine.h \
    common/plugins/resamplers/resampler_cubic.h \
    common/plugins/resamplers/resampler_dummy.h \
    common/plugins/resamplers/resampler_kernel.h \
    common/plugins/resamplers/resampler_linear.h \
    common/plugins/resamplers/resampler_raw.h \
    common/property_server/bridge_converter.h \
    common/property_server/bridge_converters.h \
    common/property_server/mremotecmd.h \
    common/property_server/property_manager.h \
    common/property_server/property_server.h \
    common/property_server/property_server_base.h \
    common/property_server/property_server_impl.h \
    common/system/system.h \
    common/system/system_control.h \
    common/system/system_drivers.h \
    common/break-cscope.h \
    common/Error.h \
    common/message_box.h \
    common/ns_autoptr.h

SOURCES += \
    cheesetracker/interface__QT/instrument_edit.cpp \
    cheesetracker/interface__QT/interface.cpp \
    cheesetracker/interface__QT/mdi_main_window.cpp \
    cheesetracker/interface__QT/order_and_defaults_editor.cpp \
    cheesetracker/interface__QT/orderlist_edit.cpp \
    cheesetracker/interface__QT/pattern_edit.cpp \
    cheesetracker/interface__QT/pattern_edit__keys.cpp \
    cheesetracker/interface__QT/pattern_edit_widget.cpp \
    cheesetracker/interface__QT/sample_edit.cpp \
    cheesetracker/interface__QT/sample_instrument_table.cpp \
    cheesetracker/interface__QT/sample_player_fdialog.cpp \
    cheesetracker/interface__QT/settings_window.cpp \
    cheesetracker/interface__QT/table_base.cpp \
    cheesetracker/interface__QT/variables_edit.cpp \
    cheesetracker/interface_binds/editor.cpp \
    cheesetracker/interface_binds/editor_commands.cpp \
    cheesetracker/interface_binds/editor_insertion.cpp \
    cheesetracker/interface_binds/editor_orderlist.cpp \
    cheesetracker/interface_binds/editor_selection.cpp \
    cheesetracker/interface_binds/editor_undo.cpp \
    cheesetracker/interface_binds/player_rt_keyboard.cpp \
    cheesetracker/interface_binds/tracker_instance.cpp \
    cheesetracker/loaders/loader_ct.cpp \
    cheesetracker/loaders/loader_it.cpp \
    cheesetracker/loaders/loader_it_info.cpp \
    cheesetracker/loaders/loader_it_instruments.cpp \
    cheesetracker/loaders/loader_it_patterns.cpp \
    cheesetracker/loaders/loader_it_samples.cpp \
    cheesetracker/loaders/loader_libaudiofile.cpp \
    cheesetracker/loaders/loader_raw.cpp \
    cheesetracker/loaders/loader_s3m.cpp \
    cheesetracker/loaders/loader_wav.cpp \
    cheesetracker/loaders/loader_xm.cpp \
    cheesetracker/player/cheeseplayer.cpp \
    cheesetracker/program__QT/cheesetracker_qt.cpp \
    cheesetracker/programs/cheeseplayer.cpp \
    cheesetracker/savers/saver_ct.cpp \
    cheesetracker/savers/saver_it.cpp \
    cheesetracker/savers/saver_raw.cpp \
    cheesetracker/savers/saver_wav.cpp \
    cheesetracker/trackercore/channel_control.cpp \
    cheesetracker/trackercore/Error.cpp \
    cheesetracker/trackercore/file_format_manager.cpp \
    cheesetracker/trackercore/file_reader.cpp \
    cheesetracker/trackercore/file_writer.cpp \
    cheesetracker/trackercore/freq_conversion.cpp \
    cheesetracker/trackercore/instrument.cpp \
    cheesetracker/trackercore/loader.cpp \
    cheesetracker/trackercore/pattern.cpp \
    cheesetracker/trackercore/player_data.cpp \
    cheesetracker/trackercore/player_data_control.cpp \
    cheesetracker/trackercore/player_data_effects.cpp \
    cheesetracker/trackercore/player_data_envelopes.cpp \
    cheesetracker/trackercore/player_data_events.cpp \
    cheesetracker/trackercore/player_data_filter.cpp \
    cheesetracker/trackercore/player_data_nna.cpp \
    cheesetracker/trackercore/player_data_notes.cpp \
    cheesetracker/trackercore/player_data_reserved.cpp \
    cheesetracker/trackercore/player_data_utils.cpp \
    cheesetracker/trackercore/player_rt_keyboard.cpp \
    cheesetracker/trackercore/sample.cpp \
    cheesetracker/trackercore/saver.cpp \
    cheesetracker/trackercore/song.cpp \
    cheesetracker/trackercore/tracker_instance.cpp \
    cheesetracker/trackercore/tracker_voice.cpp \
    cheesetracker/trackercore/voice_control.cpp \
    common/components/audio/dds_helpers.cpp \
    common/components/audio/edit_effect.cpp \
    common/components/audio/effect.cpp \
    common/components/audio/effect_chain.cpp \
    common/components/audio/effect_parameter_chain.cpp \
    common/components/audio/effect_source.cpp \
    common/components/audio/effect_source_internal.cpp \
    common/components/audio/effect_source_manager.cpp \
    common/components/audio/envelope.cpp \
    common/components/audio/iir_svf.cpp \
    common/components/audio/lfo.cpp \
    common/components/audio/midi_voice.cpp \
    common/components/audio/mixer.cpp \
    common/components/audio/mixer__buffers.cpp \
    common/components/audio/mixer_base.cpp \
    common/components/audio/resampler.cpp \
    common/components/audio/resampler_instance.cpp \
    common/components/audio/resampler_manager.cpp \
    common/components/audio/resampler_new.cpp \
    common/components/audio/ring_buffer.cpp \
    common/components/audio/sample_converison.cpp \
    common/components/audio/sample_conversion.cpp \
    common/components/audio/sample_data.cpp \
    common/components/audio/sound_driver.cpp \
    common/components/audio/sound_driver_dummy.cpp \
    common/components/audio/sound_driver_manager.cpp \
    common/components/audio/tables.cpp \
    common/components/audio/voice.cpp \
    common/components/data/config_handler.cpp \
    common/components/data/dds.cpp \
    common/components/data/dds_packer.cpp \
    common/components/data/keyboard_input.cpp \
    common/components/data/property_bridges.cpp \
    common/components/data/selection.cpp \
    common/components/file/file_access.cpp \
    common/components/file/file_browser.cpp \
    common/components/midi/engine.cpp \
    common/components/midi/engine_manager.cpp \
    common/components/midi/midi_client.cpp \
    common/components/midi/midi_client_manager.cpp \
    common/components/midi/midi_control.cpp \
    common/components/midi/midi_control_base.cpp \
    common/components/midi/midi_controls.cpp \
    common/components/midi/midi_event.cpp \
    common/components/midi/midi_out_device.cpp \
    common/components/midi/midioutdevicemanager.cpp \
    common/drivers/posix/midi_client_alsa.cpp \
    common/drivers/posix/midi_out_device_alsa.cpp \
    common/drivers/posix/midi_out_device_oss.cpp \
    common/drivers/posix/sound_driver_jack.cpp \
    common/drivers/posix/sound_driver_oss.cpp \
    common/drivers/posix/tcp_client_posix.cpp \
    common/drivers/posix/tcp_server_posix.cpp \
    common/drivers/posix/timer_rtc.cpp \
    common/drivers/posix/timer_sigalarm.cpp \
    common/drivers/rtaudio/rt_audio.cpp \
    common/drivers/rtaudio/sound_driver_rtaudio.cpp \
    common/drivers/win32/midi_out_device_win32.cpp \
    common/drivers/win32/sound_driver_win32.cpp \
    common/drivers/win32/sound_driver_winmm.cpp \
    common/drivers/win32/timer_winthreads.cpp \
    common/interface__QT/audio/audio_config.cpp \
    common/interface__QT/audio/effect_buffers_editor.cpp \
    common/interface__QT/audio/effect_chain_editor.cpp \
    common/interface__QT/audio/envelope_editor.cpp \
    common/interface__QT/audio/envelope_point_editor.cpp \
    common/interface__QT/audio/mixer_effects_manager.cpp \
    common/interface__QT/audio/note_bridge.cpp \
    common/interface__QT/audio/resampler_config.cpp \
    common/interface__QT/audio/sample_editor.cpp \
    common/interface__QT/audio/sample_editor_clipboard.cpp \
    common/interface__QT/audio/sample_editor_effects.cpp \
    common/interface__QT/audio/sample_editor_format.cpp \
    common/interface__QT/audio/sample_viewer.cpp \
    common/interface__QT/audio/sample_viewer_zoom.cpp \
    common/interface__QT/helpers/ccolor_bridge.cpp \
    common/interface__QT/helpers/ccolor_list.cpp \
    common/interface__QT/helpers/ccolor_panel.cpp \
    common/interface__QT/helpers/clist_manager.cpp \
    common/interface__QT/helpers/cspinbutton.cpp \
    common/interface__QT/helpers/font_bridge.cpp \
    common/interface__QT/helpers/keyboard_input_config.cpp \
    common/interface__QT/helpers/property_bridge_edit.cpp \
    common/interface__QT/helpers/property_bridge_edit_list.cpp \
    common/interface__QT/helpers/vertical_scrolled_window.cpp \
    common/interface__QT/helpers/vlabel.cpp \
    common/interface__QT/popups/cspindialog.cpp \
    common/interface__QT/popups/effect_select_popup.cpp \
    common/interface__QT/popups/text_area_popup.cpp \
    common/os/multireader_lock.cpp \
    common/os/mutex_lock.cpp \
    common/os/mutex_queue.cpp \
    common/os/path_data.cpp \
    common/os/tcp_client.cpp \
    common/os/tcp_server.cpp \
    common/os/threaded_class.cpp \
    common/os/timer.cpp \
    common/plugins/edit_effects/simple_edit_effects.cpp \
    common/plugins/effects/custom/chorus.cpp \
    common/plugins/effects/custom/chorus_effect.cpp \
    common/plugins/effects/custom/custom_fx_param_list.cpp \
    common/plugins/effects/custom/effect_amp.cpp \
    common/plugins/effects/custom/effect_distortion.cpp \
    common/plugins/effects/custom/effect_echo.cpp \
    common/plugins/effects/custom/effect_equalizer.cpp \
    common/plugins/effects/custom/effect_pshift.cpp \
    common/plugins/effects/custom/effect_stereo_fx.cpp \
    common/plugins/effects/custom/freeverb_effect.cpp \
    common/plugins/effects/freeverb/allpass.cpp \
    common/plugins/effects/freeverb/comb.cpp \
    common/plugins/effects/freeverb/revmodel.cpp \
    common/plugins/effects/ladspa/ladspa_effect.cpp \
    common/plugins/effects/ladspa/ladspa_effect_source.cpp \
    common/plugins/resamplers/resampler_cosine.cpp \
    common/plugins/resamplers/resampler_cubic.cpp \
    common/plugins/resamplers/resampler_dummy.cpp \
    common/plugins/resamplers/resampler_kernel.cpp \
    common/plugins/resamplers/resampler_linear.cpp \
    common/plugins/resamplers/resampler_raw.cpp \
    common/property_server/bridge_converter.cpp \
    common/property_server/bridge_converters.cpp \
    common/property_server/mremotecmd.cpp \
    common/property_server/property_manager.cpp \
    common/property_server/property_server.cpp \
    common/property_server/property_server_base.cpp \
    common/system/system.cpp \
    common/system/system_control.cpp \
    common/system/system_drivers.cpp \
    common/message_box.cpp

PKGCONFIG += sigc++-2.0

INCLUDEPATH += common
INCLUDEPATH += common/*
INCLUDEPATH += common/defines
INCLUDEPATH += cheesetracker
INCLUDEPATH += cheesetracker/*
INCLUDEPATH += cheesetracker/trackercore
INCLUDEPATH += cheesetracker/interface_binds
