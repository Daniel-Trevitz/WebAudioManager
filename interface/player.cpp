#include "player.h"
#include "utils/async_process.h"
#include "httpserver/bad_request.h"
#include "sources/cdinfo.h"

#include <iostream>

#define CD_STREAM "cdda:///dev/cdrom"

//#define LIB_VLC // LIB VLC causes a crash :(
//#define LIB_MPV // just sucks (audio cd support is poor)

#ifdef LIB_VLC
#   include <vlc/vlc.h>
#elif defined(LIB_MPV)
#   include <mpv/client.h>
#endif

void Player::dbus(std::string cmd)
{
    std::vector<std::string> args;
    args.push_back("--type=method_call");
    args.push_back("--dest=org.mpris.MediaPlayer2.vlc");
    args.push_back("/org/mpris/MediaPlayer2");
    args.push_back("org.mpris.MediaPlayer2." + cmd);
    async_process dbus_proc("/usr/bin/dbus-send", args);
    dbus_proc.exec();
    dbus_proc.wait();
}

bool Player::eject_cd()
{
    std::vector<std::string> args;
    async_process eject_proc("/usr/bin/eject", args);
    eject_proc.exec();
    eject_proc.wait();
    return true;
}

Player::Player()
#ifdef LIB_VLC
    :
    inst(libvlc_new (0, nullptr)), /* Load the VLC engine */
    mp(libvlc_media_player_new(inst))
#elif defined(LIB_MPV)
    : ctx(mpv_create()) // TODO: check return val?
#endif
{
#ifdef LIB_MPV
    // Set options here

    // now finish init
    mpv_initialize(ctx); // TODO: check return val?
    mpv_request_log_messages(ctx, "v");
#endif
}

Player::~Player()
{
#ifdef LIB_VLC
    libvlc_media_player_release (mp);
    libvlc_release (inst);
#elif defined(LIB_MPV)
    mpv_terminate_destroy(ctx);
#endif
    delete proc;
}

bool Player::play()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    if(cur_stream.empty())
        cur_stream = prev_stream;

    if(cur_stream.empty())
        return false;

#ifdef LIB_VLC
    return 0 == libvlc_media_player_play (mp);
#elif defined(LIB_MPV)

    if(!m_paused && m_playing)
        return true; // nothing to do

    if(m_paused && pause(false))
        return true;

    if(cur_stream.empty())
        return false;

    return play(cur_stream);

#else
    if(!m_paused && m_playing)
        return true; // nothing to do

    if(m_paused && pause(false))
        return true;

    if(proc)
        return pause(false);

    proc = new async_process("/usr/bin/cvlc", cur_stream);
    return (m_playing = proc->exec());
#endif
}

bool Player::play(const std::string &stream)
{
    std::cout << __PRETTY_FUNCTION__ << " stream: " << stream << std::endl;
#ifdef LIB_VLC
    if(isPlaying())
    {
        stop();
    }

    /* Create a new item */
    libvlc_media_t *m = libvlc_media_new_location (inst, stream.c_str());

    libvlc_media_player_set_media(mp, m);

    /* No need to keep the media now */
    libvlc_media_release (m);

    return 0 == libvlc_media_player_play(mp);
#elif defined(LIB_MPV)
    //if(m_playing)
    //{
        // stop();
    //}

    cur_stream = stream;
    const char *cmd[] = {"loadfile", stream.c_str(), nullptr}; //"--no-video",
    m_playing = mpv_command(ctx, cmd) >= 0;
    return m_playing;

#else
    if(proc && !stop())
        return false;

    std::cout << "Playing " << stream << std::endl;
    cur_stream = stream;
    std::vector<std::string> args;
    args.push_back("-A");
    args.push_back("alsa,none");
    args.push_back("--alsa-audio-device");
    args.push_back("default");
    args.push_back(stream);

    proc = new async_process("/usr/bin/cvlc", args);
    return (m_playing = proc->exec());
#endif
}

bool Player::play_cd()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#ifdef LIB_MPV
    const bool isCD = cur_stream == "cdda://";
    if((m_playing || m_paused) && isCD)
    {
        if(m_paused)
            return pause(false);
        return true; // nothing to do - already playing a cd
    }
    return play("cdda://");
#else
    const bool isCD = cur_stream == CD_STREAM;
    if((m_playing || m_paused) && isCD)
    {
        if(m_paused)
            return pause(false);
        return true; // nothing to do - already playing a cd
    }
    return play(CD_STREAM);
#endif
}

bool Player::pause(bool c_pause)
{
    std::cout << __PRETTY_FUNCTION__  << " Pause: " << c_pause << std::endl;
#ifdef LIB_VLC
    if(mp)
        libvlc_media_player_set_pause(mp, c_pause ? 1 : 0);
    return mp != nullptr;
#elif defined(LIB_MPV)
    int pause = c_pause ? 1 : 0;
    bool ok = mpv_set_property(ctx, "pause", MPV_FORMAT_FLAG, &pause) >= 0;
    if(ok)
        m_paused = c_pause;
    else
        m_paused = false;
    m_playing = !c_pause && ok;
    return ok;
#else
    if(c_pause == m_paused)
        ;
    if(c_pause)
        dbus("Player.Pause");
    else
        dbus("Player.Play");
    m_paused = c_pause;
    return true;
#endif
}

bool Player::changeChapter(bool fwd)
{
#ifdef LIB_VLC
    if(fwd)
        libvlc_media_player_next_chapter(mp);
    else
        libvlc_media_player_previous_chapter(mp);
    return true;
#elif defined(LIB_MPV)

#else
    if(fwd)
        dbus("Player.Next");
    else
        dbus("Player.Previous");
    return true;
#endif
}

bool Player::stop()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#ifdef LIB_VLC
    libvlc_media_player_stop (mp);
    return true;
#elif defined(LIB_MPV)
    const char *cmd[] = {"stop", nullptr};
    mpv_command(ctx, cmd);
    m_playing = false;
    m_paused = false;
    return true;
#else
    m_playing = false;
    m_paused = false;

    if(cur_stream == CD_STREAM)
        eject_cd();

    prev_stream = cur_stream;
    cur_stream = "";

    if(proc == nullptr)
        return false; // nothing to stop!
    delete proc;
    proc = nullptr;
    return true;
#endif
}

bool Player::isPaused() const
{
    return m_paused;
}

bool Player::isPlaying() const
{
#ifdef LIB_VLC
    return libvlc_media_player_is_playing(mp) == 1;
#elif defined(LIB_MPV)
    return m_playing;
#else
    if(m_playing && proc && !proc->active())
        return false;
    return proc != nullptr;
#endif
}

bool Player::isPlayingCD() const
{
#ifdef LIB_VLC
    return libvlc_media_player_is_playing(mp) == 1;
#elif defined(LIB_MPV)
    return m_playing;
#else
    if(m_playing && proc && !proc->active())
        return false;
    return proc != nullptr && cur_stream == CD_STREAM;
#endif
}

using namespace httpserver;
const std::shared_ptr<http_response> Player::render(const http_request &req)
{
    auto args = req.get_args();

    std::string status;

    if(args.count("stop"))
    {
        // Stop whatever is currently playing
        if(!stop())
            return bad_request();
    }
    else if(args.count("next"))
    {
        // Stop whatever is currently playing
        if(!changeChapter(true))
            return bad_request();
    }
    else if(args.count("prev"))
    {
        // Stop whatever is currently playing
        if(!changeChapter(false))
            return bad_request();
    }
    else if(args.count("pause"))
    {
        // Resume the previous play
        if(!pause(true))
            return bad_request();
    }
    else if(args.count("play"))
    {
        // Resume the previous play
        if(!play())
            return bad_request();
    }
    else if(args.count("play_cd"))
    {
        // Stop whatever is currently playing
        if(!play_cd())
            return bad_request();
    }
    else if(args.count("cd_info"))
    {
        CDInfo cdinfo;
        if(!cdinfo.valid()) // cddbtool failed :(
            return bad_request();
        return std::shared_ptr<http_response>(new string_response(cdinfo.toHTML()));
    }
    else if(args.count("cd_avaliable"))
    {
        if(CDInfo::cdInstalled()) status = "cd";
        else status = "no";

        return std::shared_ptr<http_response>(new string_response(status));

    }
    else if(args.count("url"))
    {
        // BUG: Exploit location! Not really that risky, but not great.
        // TODO: Sanitize inputs... but we never know what they are.
        std::cout << "size: " << args["url"].size() << std::endl;
        if(args["url"].size() < 4)
        {
            std::cout << "Returning url " << cur_stream << std::endl;
            return std::shared_ptr<http_response>(new string_response(cur_stream));
        }
        else if(!play(args["url"]))
        {
            return bad_request();
        }
    }

    if(m_paused) status = "paused";
    else if(m_playing) status = "playing";
    else status = "stopped";

    std::cout << "Player Status: " << status << std::endl;

    return std::shared_ptr<http_response>(new string_response(status));
}
