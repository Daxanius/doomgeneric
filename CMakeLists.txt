cmake_minimum_required(VERSION 3.15...4.1)
project(doomgeneric LANGUAGES C)

# OPTIONS
set(DOOMGENERIC_RESX 320 CACHE STRING "Framebuffer width (at least 320)")
set(DOOMGENERIC_RESY 200 CACHE STRING "Framebuffer height (at least 200)")
option(DOOMGENERIC_CMAP256 "8-bit palette-indexed framebuffer (OFF = 32-bit pixels)" OFF)
set(DOOMGENERIC_FRONTEND "NONE" CACHE STRING "Frontend for the demo executable (NONE = library only)")
set_property(CACHE DOOMGENERIC_FRONTEND PROPERTY STRINGS NONE SDL XLIB WIN LINUXVT)
option(DOOMGENERIC_SDL_SOUND "SDL frontend: build with SDL2_mixer sound" ON)

# CMAKE SETTINGS
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# VALIDATION
if(DOOMGENERIC_RESX LESS 320 OR DOOMGENERIC_RESY LESS 200)
    message(FATAL_ERROR "DOOMGENERIC_RESX/RESY must be at least 320x200 "
                        "(the engine renders 320x200 and upscales by an integer factor)")
endif()

if(NOT DOOMGENERIC_FRONTEND STREQUAL "NONE" AND DOOMGENERIC_CMAP256)
    message(FATAL_ERROR "The bundled frontends expect 32-bit pixels: use -DDOOMGENERIC_CMAP256=OFF")
endif()

# SOURCES
set(DG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doomgeneric)

set(DG_CORE_SOURCES
    dummy.c am_map.c doomdef.c doomstat.c dstrings.c d_event.c d_items.c d_iwad.c
    d_loop.c d_main.c d_mode.c d_net.c f_finale.c f_wipe.c g_game.c hu_lib.c
    hu_stuff.c info.c i_cdmus.c i_endoom.c i_joystick.c i_scale.c i_sound.c
    i_system.c i_timer.c memio.c m_argv.c m_bbox.c m_cheat.c m_config.c
    m_controls.c m_fixed.c m_menu.c m_misc.c m_random.c p_ceilng.c p_doors.c
    p_enemy.c p_floor.c p_inter.c p_lights.c p_map.c p_maputl.c p_mobj.c
    p_plats.c p_pspr.c p_saveg.c p_setup.c p_sight.c p_spec.c p_switch.c
    p_telept.c p_tick.c p_user.c r_bsp.c r_data.c r_draw.c r_main.c r_plane.c
    r_segs.c r_sky.c r_things.c sha1.c sounds.c statdump.c st_lib.c
    st_stuff.c s_sound.c tables.c v_video.c wi_stuff.c w_checksum.c w_file.c
    w_main.c w_wad.c z_zone.c w_file_stdc.c i_input.c i_video.c doomgeneric.c
)
list(TRANSFORM DG_CORE_SOURCES PREPEND ${DG_DIR}/)

# TARGETS
add_library(doomgeneric STATIC ${DG_CORE_SOURCES})
target_include_directories(doomgeneric PUBLIC ${DG_DIR})

# PUBLIC so anything linking the library agrees on resolution and pixel format.
target_compile_definitions(doomgeneric PUBLIC
    DOOMGENERIC_RESX=${DOOMGENERIC_RESX}
    DOOMGENERIC_RESY=${DOOMGENERIC_RESY}
)
if(DOOMGENERIC_CMAP256)
    target_compile_definitions(doomgeneric PUBLIC CMAP256)
endif()

if(UNIX)
    target_link_libraries(doomgeneric PUBLIC m)
endif()

if(MSVC)
    target_compile_options(doomgeneric PRIVATE /wd4146 /wd4996)
else()
    target_compile_options(doomgeneric PRIVATE -Wall)
endif()

install(TARGETS doomgeneric ARCHIVE DESTINATION lib)
install(FILES ${DG_DIR}/doomgeneric.h DESTINATION include)

# FRONTENDS
if(NOT DOOMGENERIC_FRONTEND STREQUAL "NONE")
    if(DOOMGENERIC_FRONTEND STREQUAL "SDL")
        find_package(SDL2 REQUIRED)
        set(DG_FRONTEND_SOURCES doomgeneric_sdl.c)
        set(DG_FRONTEND_LIBS "")
        
        target_link_libraries(doomgeneric PUBLIC SDL2::SDL2)

        if(DOOMGENERIC_SDL_SOUND)
            find_package(SDL2_mixer REQUIRED)
            list(APPEND DG_FRONTEND_SOURCES mus2mid.c i_sdlmusic.c i_sdlsound.c)
            target_link_libraries(doomgeneric PUBLIC SDL2_mixer::SDL2_mixer)
            target_compile_definitions(doomgeneric PUBLIC FEATURE_SOUND)
        endif()

    elseif(DOOMGENERIC_FRONTEND STREQUAL "XLIB")
        find_package(X11 REQUIRED)
        set(DG_FRONTEND_SOURCES doomgeneric_xlib.c)
        set(DG_FRONTEND_LIBS X11::X11)
        target_compile_definitions(doomgeneric PRIVATE NORMALUNIX LINUX SNDSERV)

    elseif(DOOMGENERIC_FRONTEND STREQUAL "LINUXVT")
        set(DG_FRONTEND_SOURCES doomgeneric_linuxvt.c mus2mid.c)
        set(DG_FRONTEND_LIBS "")
        target_compile_definitions(doomgeneric PRIVATE NORMALUNIX LINUX SNDSERV)

    elseif(DOOMGENERIC_FRONTEND STREQUAL "WIN")
        set(DG_FRONTEND_SOURCES doomgeneric_win.c)
        set(DG_FRONTEND_LIBS gdi32)
        target_compile_definitions(doomgeneric PRIVATE SNDSERV)

    else()
        message(FATAL_ERROR "Unknown DOOMGENERIC_FRONTEND '${DOOMGENERIC_FRONTEND}'")
    endif()

    list(TRANSFORM DG_FRONTEND_SOURCES PREPEND ${DG_DIR}/)

    add_executable(doomgeneric_demo ${DG_FRONTEND_SOURCES})
    target_link_libraries(doomgeneric_demo PRIVATE doomgeneric ${DG_FRONTEND_LIBS})
endif()