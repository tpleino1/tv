SFML=../SFML-2.1
CC=g++-4.6
CFLAGS=-c -g -std=c++0x -Wall -Wextra -pedantic -I $(SFML)/include -I include -I game
LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L $(SFML)/lib -Wl,-rpath=$(SFML)/lib -Wl,-rpath-link=$(SFML)/lib

SOURCES=main.cc Round.cc Application.cc MessageQueue.cc KeyBindings.cc StatMeter.cc Utilities.cc SoundPlayer.cc MusicPlayer.cc Session.cc State.cc StateStack.cc Button.cc ButtonGroup.cc GameState.cc MenuState.cc SetupState.cc PauseState.cc ShopState.cc ScoreState.cc OptionsState.cc World.cc PlayerHUD.cc ClockHUD.cc Cheat.cc ConvertUTF.c ./game/environmentController.cc ./game/weaponController.cc ./game/weapon.cc ./game/datatables.cc ./game/player.cc ./game/level.cc ./game/terrain.cc ./game/terrainStorage.cc ./game/item.cc ./game/treasure.cc ./game/characterController.cc ./game/monster.cc ./game/character.cc ./game/object.cc ./game/effect.cc

OBJECTS=main.o Round.o Application.o MessageQueue.o KeyBindings.o StatMeter.o Utilities.o SoundPlayer.o MusicPlayer.o Session.o State.o StateStack.o Button.o ButtonGroup.o GameState.o MenuState.o SetupState.o PauseState.o ShopState.o ScoreState.o OptionsState.o World.o PlayerHUD.o ClockHUD.o Cheat.o ConvertUTF.o ./game/environmentController.o ./game/weaponController.o ./game/weapon.o ./game/datatables.o ./game/player.o ./game/level.o ./game/terrain.o ./game/terrainStorage.o ./game/item.o ./game/treasure.o ./game/characterController.o ./game/monster.o ./game/character.o ./game/object.o ./game/effect.o

EXECUTABLE=mb

GAMEOBJECTS=GameState.o ./game/*.o 

all: $(SOURCES) $(EXECUTABLE)
  
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@ 

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

gg:
	rm -f $(GAMEOBJECTS)

