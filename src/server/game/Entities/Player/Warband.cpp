
#include "Player.h"
#include "StringConvert.h"
#include "Tokenize.h"

 /*********************************************************/
 /***              WARBAND SYSYTEM                      ***/
 /*********************************************************/

void Player::_LoadWarband(PreparedQueryResult result)
{
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 member1 = fields[0].Get<uint32>();
            uint32 member2 = fields[1].Get<uint32>();
            uint32 member3 = fields[2].Get<uint32>();
            uint32 member4 = fields[3].Get<uint32>();

            Warband[0] = member1;
            Warband[1] = member2;
            Warband[2] = member3;
            Warband[3] = member4;

        } while (result->NextRow());
    }
}

void Player::_SaveWarband(CharacterDatabaseTransaction trans)
{
    if (!sWorld->getBoolConfig(CONFIG_PLAYER_SETTINGS_ENABLED))
    {
        return;
    }

    for (auto itr : m_charSettingsMap)
    {
        std::ostringstream data;

        for (auto setting : itr.second)
        {
            data << setting.value << ' ';
        }

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_CHAR_SETTINGS);
        stmt->SetData(0, GetGUID().GetCounter());
        stmt->SetData(1, itr.first);
        stmt->SetData(2, data.str());
        trans->Append(stmt);
    }
}
