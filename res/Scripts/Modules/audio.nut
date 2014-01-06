Callbacks <- {}

function CreateAudioSource( stopCallback )
{
    local source = native.CreateAudioSource(name))
    Callbacks[source] <- callback
}

function OnAudioSourceStopped( source )
{
    Callbacks[source]()
}
::native.RegisterAudioSourceStopCallback(OnAudioSourceStopped)

return {
    LoadAudioBuffer = ::native.LoadAudioBuffer,
    CreateAudioSource = CreateAudioSource,
    SetAudioSourceRelative = ::native.SetAudioSourceRelative,
    SetAudioSourceLooping = ::native.SetAudioSourceLooping,
    SetAudioSourcePitch = ::native.SetAudioSourcePitch,
    SetAudioSourceGain = ::native.SetAudioSourceGain,
    EnqueueAudioBuffer = ::native.EnqueueAudioBuffer,
    FreeAudioSource = ::native.FreeAudioSource
}
