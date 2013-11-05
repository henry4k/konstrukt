function OnAudioSourceStopped( source )
{
	print("Stopping source!")
	FreeAudioSource(source)
	print("Stopped source.")
}

function SpawnAudioSource( buffer )
{
	print("Creating source!")
	local source = CreateAudioSource(OnAudioSourceStopped)
	print("Created source.")

	EnqueueAudioBuffer(source, buffer)
	PlayAudioSource(source)
}

testBuffer <- LoadAudioBuffer("Audio/Test.flac")
SpawnAudioSource(testBuffer)
