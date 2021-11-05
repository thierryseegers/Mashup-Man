![](https://github.com/thierryseegers/Mashup-Man/workflows/build/badge.svg)

# Mashup-Man

This project started from a desire of writing an original game using the knowledge I learned from my [previous experience with SFML](https://github.com/thierryseegers/Shooty).
That being said, I'm not an artist nor a composer (yet) so that means that when it comes to game assets I must make do with what I can scrounge.
This contraint is what made me think of "Mashup-Man", a re-imagining of Pac-Man with the main characters borrowed from other video games from around that era.

Here's what combining Pac-Man and Super Mario looks like :

<img width="752" alt="Mashup-Man screenshot" src="https://user-images.githubusercontent.com/1580647/139647765-bb4a1765-b448-4e01-99cf-c2dd830f09e9.png">

## Demo

Whatch short demos of this game (in [English](https://www.youtube.com/watch?v=AR5kxZ9wvZ4&list=PLetmUjNPuBak93Jj5Oc0t_LUcm6b2oBQN)... ou en [français](https://www.youtube.com/watch?v=C4fydMQ_cTA&list=PLetmUjNPuBalHSGnNIXJZMNvlfaeWsVFL) !) as it's getting implemented over time.


## Can you really do that, legally?

I found the sprites and sound effects at [The VG Resource](https://www.vg-resource.com/) and its corresponding sister sites.
You can read what they have to say about this [here](https://www.spriters-resource.com/page/faq/).

Not surprisingly, the main argument given is that you're "safe" as long as you don't make money out of it.
Personally, I never really accepted that argument.
Copyright is copyright and profitability has never been part of its basic definition.

Interestingly, though they archive sprites and sound effects, they do not archive music "[due to the legalities](https://www.sounds-resource.com/page/about/)".
They also respect video game makers' wishes to remove assests when requested.

The reason I'm saying all this is that, though they are easily available online, I have not committed in this codebase neither the sprites nor the sound effects that I'm using.
Yes, it's a "fangame" but I don't necessarily agree that because something is a fan-thing you can do what you want with the author's material and contents. Same with the "non-profit" argument. One can't copy and redistribute the Harry Potter novels regardless of whether you ask for money in return.

That said, should you copy the assets yourself in the expected folder, you will have a functioning game like shown in the video.
Further, I plan to use a different approach to reusing game assets in the future and that is to read them directly from ROMs.
That way, *you* provide the assets just like other reverse-engineered games (e.g. [OpenTTD](https://www.openttd.org/)) rely on the assets from your own copy of the original game.

## You don't have a LICENSE file!

Correct.
Consider this software as "[Source-available](https://en.wikipedia.org/wiki/Source-available_software)" for now.
That is, in essence, the [default](https://opensource.stackexchange.com/a/1721) you get on GitHub when no license file is present.

## Current state of the implementation

As of this writing, the larger parts of the game exist.
Remains to refine them all.
In rough order:

- [ ] Prettier menus
- [ ] More levels
- [ ] Configuration/Key bindings menu
- [x] A variety of enemies that use the original (or novel) chasing strategies
  - Four enemies from the Super Mario "bestiary" are present, each following one of the four original chasing strategies.
- [x] Character select screen
- [ ] Coop/Competitive mode
- [ ] Runtime debug menu using [ImGui](https://github.com/ocornut/imgui)
