# UE5 Multiplayer Sessions Plugin

A reusable Unreal Engine 5 plugin providing session management and multiplayer abstractions built on Unreal’s Online Subsystem.

## What it does
- Session creation / discovery
- Join / leave flows
- Network-safe abstractions designed for game-layer reuse

## Status
Working and actively improved. API may change as the plugin evolves.

## Installation (Plugin-only)
1. Copy the plugin folder into your project:
   `YourProject/Plugins/MultiplayerSessions/`
2. Regenerate project files (if needed) and build.
3. Enable the plugin in Unreal (Edit → Plugins).

## Notes
- Built on Unreal’s Online Subsystem.
- Tested primarily on Windows (additional platforms TBD).

## Roadmap (high-level)
- Improve error handling + logging
- Add example usage patterns
- Harden subsystem integration
- LAN connection availabilty
- Steam region distance bias removal
- TAB hold functionality to see connected players listed with ping & names
- Macro toggle for disabling debug code
- Separate repo for plugin once polished

## License
See `LICENSE`.
