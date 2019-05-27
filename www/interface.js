
/*********************************************************************/
/* Channel code */

function setChannel(channel)
{
    $.ajax({
        type:'get',
        url:'cgi-bin/channel?set=' + channel,
        success: function(data) { updateChannelHTML(data); }
    });
}

function getChannel()
{
    $.ajax({
        type:'get',
        url:'cgi-bin/channel',
        success: function(data) { updateChannelHTML(data); }
    });
}

function updateChannelHTML(channel)
{
    chtext = getChannelName(channel);

    current_channel_top = document.getElementById("current_channel_top");

    if(current_channel_top == null)
        return;


    current_channel_top = document.getElementById("current_channel_top");
    current_channel_bot = document.getElementById("current_channel_bot");

    ch_o = document.getElementById("ch-o");
    ch_0 = document.getElementById("ch-0");
    ch_1 = document.getElementById("ch-1");
    ch_2 = document.getElementById("ch-2");
    ch_3 = document.getElementById("ch-3");
    ch_4 = document.getElementById("ch-4");

    ch_o.className = "ch";
    ch_0.className = "ch";
    ch_1.className = "ch";
    ch_2.className = "ch";
    ch_3.className = "ch";
    ch_4.className = "ch";

    switch(channel)
    {
    case "0": ch_0.className = "ch_active"; break;
    case "1": ch_1.className = "ch_active"; break;
    case "2": ch_2.className = "ch_active"; break;
    case "3": ch_3.className = "ch_active"; break;
    case "4": ch_4.className = "ch_active"; break;
    default:  ch_o.className = "ch_active"; break;
    }

    if(chtext === "")
    {
        current_channel_top.innerHTML = "Select a channel";
        current_channel_bot.innerHTML = "";
    }
    else
    {
        current_channel_top.innerHTML = "Playing from";
        current_channel_bot.innerHTML = "<i><em>" + chtext + "</i></em>";
    }
}

function getChannelName(channel)
{
    switch(channel)
    {
    case "0": return "Radio";
    case "1": return "Cassette Deck";
    case "2": return "Phonograph";
    case "3": return "Multi Disc";
    case "4": return "AudioRx";
    }
    return "";
}

/*********************************************************************/
/* Volume code */

function setVolume(speaker, volumeDelta)
{
    cmd = volumeDelta === -1 ? 'dec' : 'inc';
    cmd += speaker;

    $.ajax({
        type:'get',
        url:'cgi-bin/volume?' + cmd,
        success: function(data) { updateVolumeHTML(data); }
    });
}

function setVolumeAll(volumeDelta)
{
    if(volumeDelta === -1)
      dir = 'dec';
    else
      dir = 'inc';

    $.ajax({
      type:'get',
      url:'cgi-bin/volume?' + dir,
      success: function(data) { updateVolumeHTML(data); }
    });
}

function getVolume()
{
    $.ajax({
        type:'get',
        url:'cgi-bin/volume',
        success: function(data) { updateVolumeHTML(data); }
    });
}

function mute()
{
    $.ajax({
        type:'get',
        url:'cgi-bin/volume?mute',
        success: function(data) { updateVolumeHTML(data); }
    });
}

function updateVolume()
{
    vol0 = document.getElementById("volume0").value;
    vol1 = document.getElementById("volume1").value;
    vol2 = document.getElementById("volume2").value;

    $.ajax({
      type:'get',
      url:'cgi-bin/volume?volume0=' + vol0 + '&volume1=' + vol1 + '&volume2=' + vol2,
      success: function(data) { updateVolumeHTML(data); }
    });
}

function updateVolumeHTML(data)
{
    var volume = data.split(";");
    var mute = volume[3];
    document.getElementById("volume0").value = volume[0];
    document.getElementById("volume1").value = volume[1];
    document.getElementById("volume2").value = volume[2];

    if(mute === "0")
    {
        document.getElementById("menu_mute").src = "/not_mute.png"
    }
    else
    {
        document.getElementById("menu_mute").src = "/mute.png"
    }
}

/*********************************************************************/
/* Player code */

function startCD()
{
    setChannel(4);
    $.ajax({
      type:'get',
      url:'cgi-bin/player?play_cd',
      success: function(data) { updateCD_PlayerStatus(data); }
    });
}

function startPlayer()
{
    setChannel(4);
    $.ajax({
      type:'get',
      url:'cgi-bin/player?play',
      success: function(data) { updateCD_PlayerStatus(data); }
    });
}

function pausePlayer()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?pause',
      success: function(data) { updateCD_PlayerStatus(data); }
    });
}

function stopPlayer()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?stop',
      success: function(data) { updateCD_PlayerStatus(data); }
    });
}

/*********************************************************************/
/* CD Player code */

function nextCDtrack()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?next',
      success: function(data) { }
    });
}

function prevCDtrack()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?prev',
      success: function(data) { }
    });
}

function cdAvaliable()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?cd_avaliable',
      success: function(data) { }
    });
}

function cdInfo()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?cd_info',
      success: function(data) { document.getElementById("cd_info").innerHTML = data; }
    });
}

function getPlayerStatus()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?cd_avaliable',
      success: function(data) { updateCD_PlayerStatus(data); }
    });
}

function updateCD_PlayerStatus(data)
{
    if(data === "playing")
    {
        document.getElementById("start").className = "radio_active";
        document.getElementById("pause").className = "radio";
        document.getElementById("stop").className  = "radio";
    }
    else if(data === "paused")
    {
        document.getElementById("start").className = "radio";
        document.getElementById("pause").className = "radio_active";
        document.getElementById("stop").className  = "radio";
    }
    else
    {
        document.getElementById("start").className = "radio";
        document.getElementById("pause").className = "radio";
        document.getElementById("stop").className  = "radio_active";
    }
}

/*********************************************************************/
/* Radio code */

// initialize the search
function beginRadio()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/radio?begin_nav',
      success: function(data) { updateRadio(data); }
    });
}

// Get current station information
function getRadioInformation()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/radio',
      success: function(data) { updateRadio(data); }
    });
}

// Play the station
function selectRadioStation(id)
{
    setChannel(4);
    $.ajax({
      type:'get',
      url:'cgi-bin/radio?play=' + id,
      success: function(data) { updateRadio(data); }
    });
}

// Apply a search
function selectRadioSearch(elem)
{
    $.ajax({
      type:'get',
      url:'cgi-bin/radio?select=' + elem.value,
      success: function(data) { updateRadio(data); }
    });
}

// Update the player data
function updateRadio(data)
{
    document.getElementById("radio").innerHTML = data;
}
