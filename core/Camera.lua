local Camera = {}


function Camera.setAttachmentTarget( solid )
    NATIVE.SetCameraAttachmentTarget(solid.handle)
end

function Camera.setViewTransformation( matrix )
    NATIVE.SetCameraViewTransformation(matrix.handle)
end

function Camera.setFieldOfView( fov )
    NATIVE.SetCameraFieldOfView(fov)
end


return Camera
