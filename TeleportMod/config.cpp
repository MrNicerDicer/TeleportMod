class CfgPatches {
	class TeleportMod {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_scripts", "JM_CF_Scripts"};
	};
};

class CfgMods {
	class TeleportMod {
		dir = "TeleportMod";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "TeleportMod";
		tooltip = "tooltip";
		overview = "TeleportMod";
		credits = "FreddyCruge1, MrNicerDicer";
		author = "FreddyCruge1, MrNicerDicer";
		authorID = "0";
		version = 1.1;
		extra = 0;
		type = "mod";
		dependencies[] = {"World", "Mission"};
		
		class defs {
			class worldScriptModule {
				value = "";
				files[] = {"TeleportMod/scripts/4_World"};
			};
			
			class missionScriptModule {
				value = "";
				files[] = {"TeleportMod/scripts/5_Mission"};
			};
		};
	};
};

class CfgSounds {
	sounds[] = {};
	
	class TeleportSound {
		name = "TeleportSound";
		sound[] = {"TeleportMod\Data\teleport_sound.ogg", 1, 1};
		titles[] = {};
	};
};

class CfgVehicles {

    class Inventory_Base;   // External class reference

		class TeleportModPoint : Inventory_Base {
				scope = 1;
				storageCategory = 10;
				model = "\TeleportMod\Data\ActionPoint.p3d";
				
				class DamageSystem {
					class GlobalHealth {
						class Health {
							hitpoints = 100;
							healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
						};
					};
				};
				hiddenSelections[] = {"camo"};
				hiddenSelectionsTextures[] = {"\TeleportMod\Data\ProxySkin.paa"};
			};
};