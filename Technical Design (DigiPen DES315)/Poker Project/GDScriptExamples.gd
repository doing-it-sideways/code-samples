##########################################################
# Examples of the action list in use for the poker project
# in GDScript code. Most variables are not shown
#
# Author(s): Evan O'Bryant
# Copyright © 2024 DigiPen (USA) Corporation. 
##########################################################

# ------ main.gd ------
@onready var al := $ActionList
var players # an array of players

## Moves cards from the center of players hands to a fanned out position for each hand.
func spreadHands():
	# handle player specifically
	var lastAction : Action
	
	# enable buttons once player's cards have been flipped over OR when AI hands have been dealth (auto mode only)
	var onFinish = func():
		bet.disabled = false
		fold.disabled = false
		
		# handle a button press, but instead of betting, get result from ai and play that
		if auto:
			_on_bet_button_pressed(false)
	
	# move each card to their set positions, with a bit of randomness
	if !players[0].elimed:
		for i in range(handSize):
			var card = players[0].cards[i]
		
            # each transform is specified as a marker2d in the world
			var trans := (get_node("Player1/CardPos/CardPos%d" % (i + 1)) as Marker2D).global_transform
			var pos = trans.get_origin() + Vector2(randf_range(-5, 5), randf_range(0, 5))
			var rot = rad_to_deg(trans.get_rotation()) + randf_range(-3, 3)
		
            # The transform action has a bitfield, allowing a user to choose whether to translate,
            # rotate, scale, or do any combination of the three during a transformation action.
			al.Append(Transform._new(card, 0, 0.4, Tween.EASE_OUT, Tween.TRANS_CIRC,
					  Transform.PROCESS_TRANSLATE | Transform.PROCESS_ROTATE, [pos, rot], 2))
		
			lastAction = card.flip(al, true, 0.7, 0.5, Tween.EASE_IN, Tween.TRANS_QUAD, Flip2D.FLIP_X, 2)
	
		lastAction.action_finished.connect(onFinish)
	
	# handle all other players
	for i in range(1, players.size()):
		if players[i].elimed:
			continue
		
		# move each card to their set positions, with a bit of randomness
		for j in range(handSize):
			var card = players[i].cards[j]
			
			var trans := (get_node("Player%d/CardPos/CardPos%d" % [i + 1, j + 1]) as Marker2D).global_transform
			var pos = trans.get_origin() + Vector2(randf_range(-5, 5), randf_range(-5, 5))
			var rot = rad_to_deg(trans.get_rotation()) + randf_range(-3, 3)
			
			var transform = Transform._new(card, 0, 0.4, Tween.EASE_OUT, Tween.TRANS_CIRC,
							Transform.PROCESS_TRANSLATE | Transform.PROCESS_ROTATE, [pos, rot], 2)
			al.Append(transform)
			
			if players[0].elimed:
				lastAction = transform
	
    # non-ai player lost
	if players[0].elimed:
		lastAction.action_finished.connect(onFinish)


# ------ pause.gd ------
@onready var al := $ActionList

## pause the game and open the main options menu
func pause() -> Action:
	debugWindow.activeAl = al;
	get_tree().paused = true
	
	# set the position to be off screen before the animation starts
	mainContainer.position = mainOffScreen
	mainContainer.scale = Vector2.ZERO
	
	# make visible -- shader doesn't work with fading so blur applies instantly
	modulate = Color.WHITE
	# make it so the menu processes events
	mouse_filter = Control.MOUSE_FILTER_STOP
	
	var onFinish = func():
		disableButtons(baseOptions, false)
		baseOptions[0].grab_focus()
	
	## bounce options menu to center, then blink in all buttons
    # block group 1
	al.Append(Transform._new(mainContainer, 0, 1.6, Tween.EASE_OUT, Tween.TRANS_BOUNCE,
			  Transform.PROCESS_TRANSLATE | Transform.PROCESS_SCALE, [Vector2.ZERO, Vector2.ONE], 0, [1]))
	# all blocked by previous transform action
    al.Append(Fade._new($MainContainer/VBoxContainer/Resume, 0, 0.5, Tween.EASE_IN_OUT, Tween.TRANS_SINE, Fade.FADE_IN, 1))
	al.Append(Fade._new($MainContainer/VBoxContainer/Video, 0.2, 0.5, Tween.EASE_IN_OUT, Tween.TRANS_SINE, Fade.FADE_IN, 1))
	al.Append(Fade._new($MainContainer/VBoxContainer/Audio, 0.4, 0.5, Tween.EASE_IN_OUT, Tween.TRANS_SINE, Fade.FADE_IN, 1))
	al.Append(Fade._new($MainContainer/VBoxContainer/Game, 0.6, 0.5, Tween.EASE_IN_OUT, Tween.TRANS_SINE, Fade.FADE_IN, 1))
	
	var last := Fade._new($MainContainer/VBoxContainer/Quit, 0.8, 0.5, Tween.EASE_IN_OUT, Tween.TRANS_SINE, Fade.FADE_IN, 1)
	last.action_finished.connect(onFinish)
	al.Append(last)
	
    return last # returns action for auto mode


# ------ card.gd ------

## flip the card over and toggle the label visibility
func flip(al : ActionList, enableLabel, delay, duration, _ease : Tween.EaseType, _trans : Tween.TransitionType, flags, group := 0, blocks = []) -> Action:
	var action = Flip2D._new(self, delay, duration, _ease, _trans, flags, group, blocks)
	al.Append(action)
	toggleLabelVisibility(al, enableLabel, true, delay + duration / 2, duration / 2, _ease, _trans, group, blocks)
	return action

## either "flips" the label to be visible or invisible
func toggleLabelVisibility(al : ActionList, enabled : bool, inversed : bool, delay, duration, _ease : Tween.EaseType, _trans : Tween.TransitionType, group := 0, blocks = []):
	if enabled:
		label.scale = Vector2(0, 1)
		var toScale = Vector2(1, 1) if !inversed else Vector2(-1, 1)
		al.Append(Transform._new(label, delay, duration, _ease, _trans, Transform.PROCESS_SCALE, [toScale], group, blocks))
	else:
		al.Append(Transform._new(label, delay, duration, _ease, _trans, Transform.PROCESS_SCALE, [Vector2(0, 1)], group, blocks))
