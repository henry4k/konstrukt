Callbacks <- {}

function CreateTileDefinition( name, staticTileMeshFn, staticTileSolidFn )
{
	local definition = ::native.CreateTileDefinition(name)
	Callbacks[definition] <- {
		"staticTileMeshFn": staticTileMeshFn,
		"staticTileSolidFn": staticTileSolidFn
	}
	return definition
}

function OnGenerateStaticTileMesh( definition, x, z )
{
	return Callbacks[definition].staticTileMeshFn(x,z)
}

function OnGenerateStaticTileSolid( definition, x, z )
{
	return Callbacks[definition].staticTileSolidFn(x,z)
}

return {
	"CreateTileDefinition": CreateTileDefinition,
	"GenerateMap": ::native.GenerateMap,
	"GetTileDefinitionAt": ::native.GetTileDefinitionAt,
	"SetTileAt": ::native.SetTileAt
}
