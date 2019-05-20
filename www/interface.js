
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
    document.getElementById("volume0").value = volume[0];
    document.getElementById("volume1").value = volume[1];
    document.getElementById("volume2").value = volume[2];
}

/*********************************************************************/
/* Player code */

function startPlay()
{

}

function stopPlay()
{

}

function isPlaying()
{

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
    $.ajax({
      type:'get',
      url:'cgi-bin/radio?select=' + id,
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
