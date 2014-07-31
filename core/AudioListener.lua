local AudioListener = {}


function AudioListener.setAttachmentTarget( solid )
    NATIVE.SetAudioListenerAttachmentTarget(solid.handle)
end

function AudioListener.setTransformation( matrix )
    NATIVE.SetAudioListenerTransformation(matrix.handle)
end


return AudioListener
