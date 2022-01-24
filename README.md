![headtitle](https://user-images.githubusercontent.com/16664056/150739065-bd23c05c-965d-4457-8635-9c7e7e13842c.png)

# ImpactWorld
There is an extremely easy-to-use character Interactive Components plugin for Unreal Engine, also extremely well-profiled.

## Why this plugin
This plugin was implemented with GlobalShaders and SubWorldSystem.

Those components could be used in Console|PC|iOS|Andriod, with very low cost.

After installing this plugin into your project, just add components into your character blueprint, BOOM!!! everything is DONE, without other settings.

## ImpactWaterComponent
Component to interact with Water

![ImpactWaterComponent](https://user-images.githubusercontent.com/16664056/150739419-5aa68967-962d-4a9c-83b6-0ea5f913671a.gif)

## ImpactFoliageComponent
Component to interact with Foliage

![ImpactFoliageComponent](https://user-images.githubusercontent.com/16664056/150739579-c4dc37cc-2c78-4dbe-b2da-cddbd93dce84.gif)

## How to install

```Shell
git clone https://github.com/iceprincefounder/ImpactWorld.git
```
Put ImpactWorld plugin folder into your Unreal Engine Project Plugins folder(If no folder named Plugins, create one).

![ImpactWorldInstall](https://user-images.githubusercontent.com/16664056/150742753-487b8a47-7afb-4af1-8605-f1000560487f.png)

Start your project, add components into your character blueprint, then, Enjoy!

![OneStepSetting](https://user-images.githubusercontent.com/16664056/150742714-bfa534c5-8395-49ca-88ca-5d6e6a44889e.png)

## How to set parameters
There is global parameters:

MainMenu Edit -> Project Settings -> Plugins -> ImpactWorld

![ProjectSettings](https://user-images.githubusercontent.com/16664056/150742720-b9b568b9-7f0b-49b8-87c8-962481911d72.png)
Set ImpactWaterComponent at Character Blueprints:

![ImpactWaterComponentSetting](https://user-images.githubusercontent.com/16664056/150742745-04f0f8dc-7bf5-4bdd-b9dd-00c4f41acfe9.png)
Set ImpactFoliageComponent at Character Blueprints:

![ImpactFoliageComponentSetting](https://user-images.githubusercontent.com/16664056/150742736-1a398ed9-fab9-4f8a-837a-ca29a4db9be6.png)

## Material Function for your Assets
You need add a MaterialFunction Node into your assets like water plane material or foliage material.

For Water Plane:

![ImpactWaterMaterialSetting](https://user-images.githubusercontent.com/16664056/150742749-17c6f9ca-88ac-4cf1-b782-5aafa44a85ed.png)

For Foliage Material:

![ImpactFoliageMaterialSetting](https://user-images.githubusercontent.com/16664056/150742738-45b8543c-6f97-44f0-8daa-fe2c910a6579.png)

## Support Version
```
Unreal Engine 4.26+
```

Feel FREE to use it in your Project, Hope it helped.