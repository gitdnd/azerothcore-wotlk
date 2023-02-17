
DELIMITER ;;
DROP PROCEDURE IF EXISTS create_elk_dialogue_text;
CREATE PROCEDURE create_elk_dialogue_text ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	CREATE TABLE elk_dialogue_text (
    id int NOT NULL AUTO_INCREMENT,
    text_name varchar(64) NOT NULL,
    dialogue TEXT NOT NULL,
	UNIQUE (id),
	UNIQUE (text_name)
   );
END;;
CALL create_elk_dialogue_text();;
DROP PROCEDURE  create_elk_dialogue_text;;

TRUNCATE TABLE elk_dialogue_text;
DROP PROCEDURE IF EXISTS add_elk_dialogue_text;
SET @dialogueAdded = 1;


CREATE PROCEDURE add_elk_dialogue_text (IN text_nameP VARCHAR(64), dialogueP TEXT)
BEGIN
   DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	REPLACE INTO `elk_dialogue_text` (`id`, `text_name`, `dialogue`) VALUES
	(@dialogueAdded, text_nameP, dialogueP);
	REPLACE INTO `npc_text` (`ID`, `text0_0`, `Probability0`) VALUES
	(@dialogueAdded + 1000000, dialogueP, 1);
	SET @dialogueAdded = @dialogueAdded + 1;
END;;




CALL add_elk_dialogue_text ("DOTDOTDOT", "...");


CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_TALK_1", "I hope you enjoyed the last sleep of your jaunt in Sunstrider Isle, as it won't be able to host you anymore. The incident involving Felendren has gotten worse than initially estimated and has festered beyond any of our possessed remedies.");
CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_DIALOGUE_1A", "It is always sad when pursuit of knowledge is cut short. I should consider solving this my thesis then.");
CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_DIALOGUE_1B", "This was supposed to be a leisurely excursion in which I wore only a chrisom, but I wouldn't mind doing anything to assist before I leave.");
CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_DIALOGUE_1C", "Experiencing this incident to fawn over details of how the Sin'dorei perservere through uncommon circumstances in this cosmos interests me. Could you tell me about it?");
CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_TALK_2", "Malevolent destruction protrude from Falthrien Academy as a worm from a corpse of an animal slaughtered by a predator it could not understand. It is of origin too alien for our experts to ascertain correctly, tainting the Arcane and Fel aura of the isle with deathly fashion.$b$bThese rumbling qualities correlate with death and mingle as symbiosis of blossoms with bees, yet bearing synthesis that there is no well fashioned theorems on.$b$bThe destruction of the Sunwell by the Scourge sent adrift numerous disturbances that push us towards annihilation teetering on hideous oblivion, and this here unnatural event is no less cruel nor merciless.");
CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_DIALOGUE_2A", "<Continue>");
CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_TALK_3", "If you wish to aid us in rebuilding our society from this hope-dispersing disaster and pull together what high hopes we had for Sunstrider Isle then you should learn how to deal with cunning combat.$b$bThe Burning Crystals forged from the knowledge hither brought from our bretheren in Outland were responsible for keeping the Mana Wyrms in Quel'Thalas tame. $b$bThey have been disturbed and made a target for culling. Like any intelligent creature, if they can fight in packs when threatened, they will, so I recommend you bring with you an ally to split the pressure.");
CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_DIALOGUE_3A", "<Continue>");
CALL add_elk_dialogue_text ("MAGISTRIX_ERONA_TALK_4", "If you wish to aid us in the purging of this influence I recommend you arm and gear yourself in Thelyrn Sister's store, armor is especially valuable against smaller attacks in large quantities while for bigger attacks you're better off deflecting or dodging. And a good weapon will help you even if you prefer spells, as attacks are always useful for restoring mana.");
CALL add_elk_dialogue_text ("FELENDREN_INRO_1", "<Your long ears are harassed by wails that shimmer like repeatedly shattering glass in a large temple; buzzing like a rising sun that can never Dawn, and the echoes of your own thoughts on this subject>$B<Your long ears are harassed by wails that shimmer like repeatedly shattering glass in a large temple; buzzing like a rising sun that can never Dawn>$B<Your long ears are harassed by wails that shimmer like repeatedly shattering glass in a large temple>$B<Your long ears are harassed>$BWhat do you want, novice? Are you here to help me past wits end?");
CALL add_elk_dialogue_text ("FELENDREN_DIALOGUE_1A", "Felendren, you are a grave menace to the safety and prosperity of the Sin'dorei and I've come to eliminate you.");
CALL add_elk_dialogue_text ("FELENDREN_DIALOGUE_1B", "What are you doing here you pale skinned, wretched rat?");
CALL add_elk_dialogue_text ("FELENDREN_DIALOGUE_1C", "<Attack> I'm going to shatter your ribs and throw you off this peak you disgrace.");
CALL add_elk_dialogue_text ("FELENDREN_DIALOGUE_a1A", "Why were you here anyway, bastard?");
CALL add_elk_dialogue_text ("FELENDREN_DIALOGUE_a1B", "<Attack> I finished up my preparation, time to die...");
CALL add_elk_dialogue_text ("FELENDREN_TALK_2_R1A", "Rudeness doesn't befit you. I am here to relieve my torement. My needs, like any other of our people's needs, they have become great, and I must feed. Though I'm troubled by a slight malaise you see...");
CALL add_elk_dialogue_text ("FELENDREN_TALK_2_R1B", "What a foul respect you hold for elder nobility, caitiff swine.");
CALL add_elk_dialogue_text ("FELENDREN_DIALOGUE_2A", "What happens to be your problem");
CALL add_elk_dialogue_text ("FELENDREN_DIALOGUE_2B", "Speak quickly or you'll be a skinned rat soon.");
CALL add_elk_dialogue_text ("FELENDREN_DIALOGUE_2C", "<Attack> Never mind, you're just another pathetic drain on Mana.");
CALL add_elk_dialogue_text ("SOLANIAN_ORB_INTRO", "<The Orb radiates a sort of... intellectual energy. If you destroyed it, you could devour some of it's power.>");
CALL add_elk_dialogue_text ("SOLANIAN_SCOURGE_SCROLL_INTRO", "<The scroll smells corrosive, rotten and damp.>");
CALL add_elk_dialogue_text ("SOLANIAN_SCOURGE_SCROLL_READ_1", "'...may the bodies of those deceased carry on their use for our Master and King, and may the Damned be free of the shackles of life forever more...'");
CALL add_elk_dialogue_text ("SOLANIAN_SCOURGE_SCROLL_READ_2", "'...as the Damned rise from the dead to fulfill their duties to the KING of all that is Death, ruler beyond Life, they shall partake in the grace of His Will and breathe the air of his cold plague'");
CALL add_elk_dialogue_text ("SOLANIAN_SCOURGE_SCROLL_READ_3", "<...may the Spirited Inamorata sing cold praise of his Eternal Wisdom and Strength, and may their echoes chill our homely bones and freeze the blood of the defiers...>");
CALL add_elk_dialogue_text ("SOLANIAN_SCOURGE_SCROLL_READ_4", "<...Learn form your Masters...>");
CALL add_elk_dialogue_text ("SOLANIAN_INTRO_1", "Congragulations on your early Alumna, $n. Was it deserved?");
CALL add_elk_dialogue_text ("SOLANIAN_TALK_1", "Good. I'm going to need you to return my Scrying Orb, one of my rare Scourge Magic Scrolls, and my notes, libriform. The Falthrien disaster shall be rectified, and as our pheonix blood wills, it shall rise from the ashes! Go forth!");
CALL add_elk_dialogue_text ("SOLANIAN_TALK_2", "The Sunspire used to hold a beautiful vantage of the Sunwell... long before. Our lives have been turned upside down, $n, but we shall nonetheless carry on. It is our way; we are survivors.$b$b
If you are to survive this upheaval... and carry on thine birthright of this russet land, then you MUST learn how to thrive in the depths of this buzzardly world. All Sin'dorei must. Master your insatiable hungering for magic before it throws you into a slough of despair.");
CALL add_elk_dialogue_text ("SOLANIAN_TALK_3", "Worry not $n, your weak blade shall be saved from shattering by the bond of thine friends and allies. You have made friends while here, haven't you? If not, now is the time to start.");
CALL add_elk_dialogue_text ("SOLANIAN_TALK_4", "We suspect Felendren is responsible for Sunstrider Isle's plight. He has left me and my friends wondering over the sorcerry he is commanding, questioning his motives for what could have been the cause.");
CALL add_elk_dialogue_text ("SOLANIAN_TALK_5", "Very well, I'm going to need you to return my Scrying Orb, my collection of Scourge Magic Scrolls, and my notes, libriform.");
CALL add_elk_dialogue_text ("SOLANIAN_SCOURGE_CROLL_TAKE", "<Take Scroll of Scourge Magic> Know thy enemy.");
CALL add_elk_dialogue_text ("SOLANIAN_SCOURGE_SCROLL_BURN", "<Burn Scroll> Curse the Scourge.");
CALL add_elk_dialogue_text ("SOLANIAN_SCOURGE_SCROLL_READ", "<Read the Scroll>");
CALL add_elk_dialogue_text ("SOLANIAN_ORB_TAKE", "<Take Orb> I'm taking this back this.");
CALL add_elk_dialogue_text ("SOLANIAN_ORB_SHATTER", "<Shatter Orb> I do need more power.");
CALL add_elk_dialogue_text ("SOLANIAN_ORB_PONDER", "<Ponder your reflection in the orb>");
CALL add_elk_dialogue_text ("TZA'ZUHEN_INTRO_1A", "A pup of withering Sin'dorei seed bravely walks into a pool of bloody chaff where it belongs, filled with bravery and crimson ecstasy it knows it's place as ritual chow for the beasts.$b$bOr perhaps you wish to sow and reap life-past-life so to zealously devour your fading bretheren, to release the beating necrotic nature already auditory pulsatining in the womb of your mind for... this not too uncommon request.$b$bSo why do you stand before me, enemy?");
CALL add_elk_dialogue_text ("TZA'ZUHEN_INTRO_1B", "You dared come before me just to flee terrorized in your boots like the howling moon from dawn over the hill, youngling. Now you return, of stifled cauterized spirit defiant of the disquet that you hosted out of ignorance which mercifully let you live this far, even with endless wounds opening to the speeding maggots hiding in the wind eminating from mounds of melting, burning bodies that are no longer bodies.$b$bBrave as the sun which abandons all to darkest darkness every day yet always returning, always with the holiest certainty and greatest bravery, or at least until the master's hand reaches up to suffocate it with revelations of even it's own futility; so are you now here.$b$bFor what purpose is it?");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_1A", "Good speech, Necromancer. You are the one responsible for splitting my beautiful Quel'Thalas in half I assume? You don't seem to be undead.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_1B", "Are you Tza'zuhen, the still-living mongrel traitor-wizard whose mutt-skull I've come to split? ");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_1C", "<Attack> I did not come here to listen to you rattle, bone-weaver. Your delirious shrills will fade into a blind void where no sentient beasts that were before separated by oceans can be sewn together into alchemical horrors and save you from your penance.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_1D", "<Attack> Your feet I shall cut off, your innards cauterize, fingers I will nail together and then I'll hang you with hooks through your clavicles so all may look at you and know the Scourge is ending.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_TALK_2", "The terrible name I was given by my master to take from into the grave and past, into the endless singularity of deaths and delirious truths, is Tza'zuhen, fermenter and sticher of the fluids of the skull, spine and heart with the coldest steel.$b$bI was anointed by the spirit of the then future Arch-Lich Kel'Thuzad as I crafted chains for his sacred chrysalis into the hand sculpted divine masterwork deserving of his eternal reward that your precious Sunwell powered.$b$bIt is now my duty to bleed the blood-ears bone-dry so your permanent evaporation into dry hot blight can please the Lord's dark desire.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_2A", "I am here to command you to cease your aggression or face the wrath of Silvermoon now rather than later. These are our last and only terms. Accept them, or perish. Were this threat not well backed, I would not be standing in front of you, armed and stoic.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_2B", "Kingpin of the Dead Scar, the grip of the Scourge in Quel'Thalas is weakening and reinforcements from three of four of Azeroth's great powers are coming sooner rather than later. I request you make this easeir for both me and yourself and surrender, sparing me my people's wounds, lives and grief, and you the wrath of your tyrants.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_2C", "<Attack> Death ye weave, death ye desire, death ye shall be given.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_2D", "<Attack> The feelings you hold for the Sin'dorei, I hold for you a thousand fold worse.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_TALK_3", "What warm kindness you offer to my sore head, relishable elf. Why, I'd almost even forgotten my covenant of fealties over this saccharine display from someone's last breath.$b$bFear of death, fear of secret monsters hiding in the blackness past one's burial, that is something only the uninitiated with instincts that are animalistic and minds that are vegetative have friend, my only concern is when I will bring glory to my King.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_3A", "<Truce> It is not a metter of when, your forces are being picked apart and your unjust extermination will not succeed. If you pull back to spare the lives of our men, instead becoming someone else's problem, it would benefit both parties.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_3B", "<Attack> That was mere formality, now we shall do battle as civilized folk.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_TALK_4", "<The Necromancer pauses, seemingly both relaxed and frustrated as if trying to consciously get mad in the face of truth>$b$bThe Scourge, we are not merely a pitiful army you dirty squirrel excrement ape. We are an infinite force of locusts that will devour the living and bring the truth of endless death to all.$b$bWhat makes you dare think I worry about losing donated bodies when your lives have to exert wee efforts to perservere into the future against passive inevitability itself?");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_4A", "<Choice, Truce> Your armies are not forces of nature. If the Scourge could supply a truly infinite army it would have already conquered everything, and it wouldn't have struggled to hold as much of Quel'Thalas as it has. Instead we've risen like Pheonixes, the Plaguelands are cleansing, many are abandoning the Domination to join the Forsaken. If you wish to die and burn for your petty zeal, I cannot stop you and can only recommend you do not insult the intelligence of both your faction as well as my own.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_DIALOGUE_4B", "<Attack> I've given you ample opportunity to surrender and retreat your bodies thinking your life and vitality might let you see some reason, yet you insult me by chatting. I can no longer risk you stalling nor escaping.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_BUBBLE_5", "Does your speckless conscience speak reason? Or are you simply playing into my fears with your superior wit of strategy.");
CALL add_elk_dialogue_text ("TZA'ZUHEN_BUBBLE_6", "I shall retreat as to your advice, for now. Should you ever wish to taste the fruit of the Scourge, I will remember to vouch for you turning into more than a ghoul.");
