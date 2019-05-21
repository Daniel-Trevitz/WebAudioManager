
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
    current_channel_bot = document.getElementById("current_channel_bot");

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
      success: function(data) { }
    });
}

function startPlayer()
{
    setChannel(4);
    $.ajax({
      type:'get',
      url:'cgi-bin/player?play',
      success: function(data) { }
    });
}

function pausePlayer()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?pause',
      success: function(data) { }
    });
}

function stopPlayer()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?stop',
      success: function(data) { }
    });
}

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

function debugPlayer()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?debug',
      success: function(data) { }
    });
}

function cdInfo()
{
    $.ajax({
      type:'get',
      url:'cgi-bin/player?cd_info',
      success: function(data) { }
    });
}

/*********************************************************************/
/* Player code */

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
