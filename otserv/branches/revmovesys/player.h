//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "definitions.h"
#include "creature.h"
#include "container.h"
#include "cylinder.h"

#include <vector>
#include <ctime>
#include <algorithm>
#include "templates.h"

class Protocol;

enum skills_t {
	SKILL_FIST=0,
	SKILL_CLUB=1,
	SKILL_SWORD=2,
	SKILL_AXE=3,
	SKILL_DIST=4,
	SKILL_SHIELD=5,
	SKILL_FISH=6
};

enum skillsid_t {
	SKILL_LEVEL=0,
	SKILL_TRIES=1,
	SKILL_PERCENT=2
};

enum playerinfo_t {
	PLAYERINFO_LEVEL,
	PLAYERINFO_LEVELPERCENT,
	PLAYERINFO_HEALTH,
	PLAYERINFO_MAXHEALTH,
	PLAYERINFO_MANA,
	PLAYERINFO_MAXMANA,
	PLAYERINFO_MANAPERCENT,
	PLAYERINFO_MAGICLEVEL,
	PLAYERINFO_MAGICLEVELPERCENT,
	PLAYERINFO_SOUL,
};

enum playersex_t {
	PLAYERSEX_FEMALE = 0,
	PLAYERSEX_MALE = 1,
	PLAYERSEX_OLDMALE = 2
};

//0 = None, 1 = Sorcerer, 2 = Druid, 3 = Paladin, 4 = Knight
enum playervoc_t {
	VOCATION_NONE = 0,
	VOCATION_SORCERER = 1,
	VOCATION_DRUID = 2,
	VOCATION_PALADIN = 3,
	VOCATION_KNIGHT = 4
};

enum freeslot_t{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_CONTAINER
};

enum trade_state {
	TRADE_NONE,
	TRADE_INITIATED,
	TRADE_ACCEPT,
	TRADE_ACKNOWLEDGE
};


typedef std::pair<unsigned char, Container*> containerItem;
typedef std::vector<containerItem> containerLayout;
typedef std::map<unsigned long, Container*> DepotMap;
typedef std::map<unsigned long,long> StorageMap;
typedef std::set<unsigned long> VIPListSet;

//////////////////////////////////////////////////////////////////////
// Defines a player...

class Player : public Creature, public Cylinder
{
public:
	Player(const std::string& name, Protocol* p);
	virtual ~Player();
	void setGUID(unsigned long _guid) {guid = _guid;};
	unsigned long getGUID() const { return guid;};
	virtual unsigned long idRange(){ return 0x10000000;}
	virtual int getThrowRange() const {return 1;};
	static AutoList<Player> listPlayer;
	void removeList();
	void addList();
	void kickPlayer();
	
	freeslot_t getFreeSlot(Container **container,unsigned char &slot, const Item* item);
	Container* getFreeContainerSlot(Container *parent);
	
	containerLayout::const_iterator getContainers() const { return vcontainers.begin();}
	containerLayout::const_iterator getEndContainer() const { return vcontainers.end();}
	
	Container* getContainer(uint32_t cid);
	uint32_t getContainerID(const Container* container) const;
	void addContainer(uint32_t containerid, Container *container);
	void closeContainer(uint32_t containerid);
	
	void addStorageValue(const unsigned long key, const long value);
	bool getStorageValue(const unsigned long key, long &value) const;
	inline StorageMap::const_iterator getStorageIteratorBegin() const {return storageMap.begin();}
	inline StorageMap::const_iterator getStorageIteratorEnd() const {return storageMap.end();}
	
	int getLevel() const {return level;}
	int getHealth() const {return health;}
	int getMana() const {return mana;}
	int getMagicLevel() const {return maglevel;}
	playersex_t getSex() {return sex;}
	bool gainManaTick();
	bool gainHealthTick();
	
	const std::string& getName() const {return name;};
	const std::string& getGuildName() const {return guildName;};
	unsigned long getGuildId() const {return guildId;};
	
	int getPlayerInfo(playerinfo_t playerinfo) const;
	int getSkill(skills_t skilltype, skillsid_t skillinfo) const;
	std::string getSkillName(int skillid);
	void addSkillTry(int skilltry);
	void addSkillShieldTry(int skilltry);
	
	unsigned long getExperience() const {
		return experience;
	}

	double getCapacity() const {
		if(access == 0) {
			return capacity;
		}
		else
			return 0.00;
	}
	
	virtual int getLostExperience() {
		return (int)std::floor(((double)experience * 0.1));
	}
	
	double getFreeCapacity() const {
		if(access == 0) {
			return std::max(0.00, capacity - inventoryWeight);
		}
		else
			return 0.00;
	}
	
	time_t getLastLoginSaved() const { return lastLoginSaved; };
	const Position& getLastLoginPosition() {return lastLoginPosition;};
	
	void updateInventoryWeigth();
	
	Item* getInventoryItem(slots_t slot) const;
	//Item* GetDistWeapon() const;
	
	void addManaSpent(unsigned long spent);
	void addExperience(unsigned long exp);
	virtual int getWeaponDamage() const;
	virtual int getArmor() const;
	virtual int getDefense() const;
	unsigned long getMoney();

	//bool substractMoney(unsigned long money);
	//bool substractMoneyItem(Item *item, unsigned long money);
		
	unsigned long eventAutoWalk;
	
	//items
	containerLayout vcontainers;
	void preSave();
	
	/*
	virtual void useThing(){
		//std::cout << "Player: useThing() " << this << std::endl;
		useCount++;
	};
	
	virtual void releaseThing(){
		useCount--;
		//std::cout << "Player: releaseThing() " << this << std::endl;
		if (useCount == 0)
			delete this;
	};
	*/

	unsigned long getIP() const;
	Container* getDepot(unsigned long depotId);
	bool addDepot(Container* depot,unsigned long depotIs);
	//depots	
	DepotMap depots;
	long max_depot_items;
	
	//virtual void removeDistItem();
	fight_t getFightType();
	subfight_t getSubFightType();
	
	bool CanSee(int x, int y, int z) const;
	
	void sendIcons();  
	void sendChangeSpeed(Creature* creature);
	void sendToChannel(Creature *creature, SpeakClasses type, const std::string &text, unsigned short channelId);
	virtual void sendCancel(const char *msg) const;
	virtual void sendCancelWalk() const;
	//int sendInventory(unsigned char sl_id);
	void sendStats();
	void sendTextMessage(MessageClasses mclass, const char* message) const;
	void sendTextMessage(MessageClasses mclass, const char* message,const Position &pos, unsigned char type) const;
	void sendPing();
	void sendCloseContainer(unsigned char containerid);
	void sendContainer(unsigned char index, Container *container);
	void sendTextWindow(Item* item,const unsigned short maxlen, const bool canWrite);  
	void sendDistanceShoot(const Position &from, const Position &to, unsigned char type);
	void sendMagicEffect(const Position &pos, unsigned char type);
	void sendAnimatedText(const Position &pos, unsigned char color, std::string text);
	void sendCreatureHealth(const Creature *creature);
	void sendTradeItemRequest(const Player* player, const Item* item, bool ack);
	void sendCloseTrade();
	void receivePing();
	void flushMsg();
	
	void die();      //player loses exp/skills/maglevel on death
	
	virtual bool isAttackable() const { return (access == 0); };
	virtual bool isPushable() const;
	virtual void dropLoot(Container *corpse);
	virtual int getLookCorpse();
	bool NeedUpdateStats();
	
	virtual std::string getDescription(uint32_t lookDistance) const;

	//tiles
	//void AddRemoveThing(NetworkMessage& msg, const Position& pos, int stackpos);
	//void AddAppearThing(NetworkMessage& msg, const Position& pos);
	//void AddTileUpdated(NetworkMessage& msg, const Position& pos);

	//container
	void sendAddContainerItem(const Container* container, const Item *item);
	void sendUpdateContainerItem(const Container* container, uint8_t slot, const Item* item);
	void sendRemoveContainerItem(const Container* container, uint8_t slot);
	
	//inventory
	void sendAddInventoryItem(slots_t slot, const Item* item);
	void sendUpdateInventoryItem(slots_t slot, const Item* item);
	void sendRemoveInventoryItem(slots_t slot);
	
	void setAcceptTrade(bool b);
	bool getAcceptTrade() {return (tradeState == TRADE_ACCEPT);};
	Item* getTradeItem() {return tradeItem;};
	
	void notifyLogIn(Player* player);
	void notifyLogOut(Player* player);
	bool removeVIP(unsigned long guid);
	bool addVIP(unsigned long guid, std::string &name, bool isOnline, bool interal = false);
	
	VIPListSet VIPList;
	
protected:
	void sendCancelAttacking();
	void addSkillTryInternal(int skilltry,int skill);
	//virtual void onCreatureAppear(const Creature *creature);
	//virtual void onCreatureDisappear(const Creature *creature, unsigned char stackPos, bool tele);
	//virtual void onTileUpdated(const Position &pos);
	
	void onThingAppear(const Thing* thing);
	void onThingDisappear(const Thing* thing, unsigned char stackPos);

	virtual void onCreatureTurn(const Creature *creature, unsigned char stackpos);
	virtual void onCreatureSay(const Creature *creature, SpeakClasses type, const std::string &text);
	virtual void onCreatureChangeOutfit(const Creature* creature);
	virtual void onTeleport(const Creature *creature, const Position *oldPos, unsigned char oldstackpos); 
	virtual int onThink(int& newThinkTicks);
	
	//ground to ground
	virtual void onThingMove(const Creature *creature, const Thing *thing, const Position *oldPos,
		unsigned char oldstackpos, unsigned char oldcount, unsigned char count);

	//
	virtual ReturnValue __moveThingTo(Creature* creature, Cylinder* toCylinder, int32_t index, Thing* thing, uint32_t count);
	virtual ReturnValue __queryMaxCount(int32_t index, const Thing* thing, uint32_t count,
		uint32_t& maxQueryCount, bool checkCapacity);

	virtual ReturnValue __addThing(Thing* thing);
	virtual ReturnValue __addThing(uint32_t index, Thing* thing);

	virtual ReturnValue __updateThing(Thing* thing, uint32_t count);
	virtual ReturnValue __updateThing(uint32_t index, Thing* thing);

	virtual ReturnValue __removeThing(Thing* thing);
	virtual ReturnValue __removeThing(Thing* thing, uint32_t count);

	virtual int32_t __getIndexOfThing(const Thing* thing) const;
	virtual Thing* __getThing(uint32_t index);

	virtual void __internalAddThing(Thing* thing);
	virtual void __internalAddThing(uint32_t index, Thing* thing);

protected:
	Protocol* client;
	int useCount;
	unsigned long experience;
	
	playervoc_t vocation;
	playersex_t sex;
	int food;
	
	double inventoryWeight;
	double capacity;
	
	bool SendBuffer;
	long internal_ping;
	long npings;
	
	char fightMode, followMode;
	
	//account variables
	int accountNumber;
	std::string password;
	time_t lastlogin;
	time_t lastLoginSaved;
	Position lastLoginPosition;
	unsigned long lastip;
	
	//inventory variables
	Item* items[11]; //equipement of the player
	
	//player advances variables
	unsigned int skills[7][3];
	
	//reminder: 0 = None, 1 = Sorcerer, 2 = Druid, 3 = Paladin, 4 = Knight
	static const int CapGain[5];          //for level advances
	static const int ManaGain[5];
	static const int HPGain[5];
	static const int gainManaVector[5][2];
	static const int gainHealthVector[5][2];
	unsigned short manaTick;
	unsigned short healthTick;
	
	unsigned char level_percent;
	unsigned char maglevel_percent;

	//trade variables
	unsigned long tradePartner;
	trade_state tradeState;
	Item* tradeItem;
	
	//autowalking
	std::list<Direction> pathlist;
	
	//cache some data
	struct SkillCache{
		unsigned int tries;
		int level;
		playervoc_t vocation;
	};
	
	SkillCache SkillAdvanceCache[7][2];
	struct SentStats{
		int health;
		int healthmax;
		unsigned long experience;
		int level;
		double freeCapacity;
		int mana;
		int manamax;
		int manaspent;
		int maglevel;
	};
	
	SentStats lastSentStats;

	std::string name;	
	unsigned long guid;
	
	//guild variables
	unsigned long guildId;
	std::string guildName;
	std::string guildRank;
	std::string guildNick;
	
	StorageMap storageMap;
	
	struct MoneyItem{
		Item* item;
		freeslot_t location;
		int slot;
		Container *parent;
	};

	typedef std::multimap<int, struct MoneyItem*, std::less<int> > MoneyMap;
	typedef MoneyMap::value_type moneymap_pair;
	
	//for skill advances
	unsigned int getReqSkillTries (int skill, int level, playervoc_t voc);
	
	//for magic level advances
	unsigned int getReqMana(int maglevel, playervoc_t voc); 
	
	friend OTSYS_THREAD_RETURN ConnectionHandler(void *dat);
	
	friend class Game;
	friend class ActionScript;
	friend class Commands;
	friend class Map;
	friend class IOPlayerXML;
	friend class IOPlayerSQL;
};


#endif // __PLAYER_H__
