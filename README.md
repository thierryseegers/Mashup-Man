# MASHUP-MAN

This project started from a desire of writing an original game using the knowledge I learned from my [previous experience with SFML](https://github.com/thierryseegers/Shooty).
That being said, I'm not an artist nor a composer (yet) so that means that when it comes to game assets I must make do with what I can scrounge.
This contraint is what made me think of "Mashup-Man", a re-imagining of Pac-Man with the main characters borrowed from other games around that era.


## Demo

What me play my game [here]()!


## Can you really do that, legally?

I found the sprites and sound effects at [The VG Resource](https://www.vg-resource.com/) and its corresponding sister sites.
You can read what they have to say about this [here](https://www.spriters-resource.com/page/faq/).

Not surprisingly, the main argument given is that you're "safe" as long as you don't make money out of it.
Personally, I never really accepted that argument.
Copyright is copyright and profitability has never been part of its basic definition.

Interestingly, though they archive sprites and sound effects, they do not archive music "[due to the legalities](https://www.sounds-resource.com/page/about/)".
They also respect video game makers' wishes to remove assests when requested.

The reason I'm saying all this is that, though they are easily available online, I have not committed in this codebase neither the sprites nor the sound effects that I'm using.
Yes, it's a "fangame" but I don't necessarily agree that because something is a fan-thing you can do what you want with the author's material and contents. Same with the "non-profit" argument. One can't copy and redistribute the Harry Potter stories regardless of whether you ask for money in return.

That said, should you copy the assets yourself in the expected folder, you will have a functioning game like shown in the video.
Further, I plan to use a different approach to reusing game assets in the future and that is to read them directly from ROMs.
That way, *you* provide the assets just like other reverse-engineered games (e.g. [OpenTTD](https://www.openttd.org/)) rely on the assets from your own copy of the original game.


## Current state of the implementation

As of this writing, the larger parts of the game exist.
Remains to refine them all.
In rough order:

[] Prettier menus
[] More levels
[] Configuration/Key bindings menu
[] Read game assets like sprites and sound effects from ROMs
[] A variety of enemies that use the original (or novel) chasing strategies
[] Character select screen
[] Coop/Competitive mode
