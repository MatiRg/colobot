/*
 * This file is part of the Colobot: Gold Edition source code
 * Copyright (C) 2001-2017, Daniel Roux, EPSITEC SA & TerranovaTeam
 * http://epsitec.ch; http://colobot.info; http://github.com/colobot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */

#include "level/scoreboard.h"

#include "level/parser/parserline.h"

#include "level/robotmain.h"

#include "object/object.h"

#include "ui/displaytext.h"

#include <boost/lexical_cast.hpp>

void CScoreboard::CScoreboardRule::Read(CLevelParserLine* line)
{
    this->score = line->GetParam("score")->AsInt();
}

void CScoreboard::CScoreboardKillRule::Read(CLevelParserLine* line)
{
    CScoreboardRule::Read(line);
    CObjectCondition::Read(line);
}

void CScoreboard::CScoreboardEndTakeRule::Read(CLevelParserLine* line)
{
    CScoreboardRule::Read(line);
    this->team = line->GetParam("team")->AsInt(0);
}

void CScoreboard::AddKillRule(std::unique_ptr<CScoreboardKillRule> rule)
{
    m_rulesKill.push_back(std::move(rule));
}

void CScoreboard::AddEndTakeRule(std::unique_ptr<CScoreboardEndTakeRule> rule)
{
    m_rulesEndTake.push_back(std::move(rule));
}

void CScoreboard::ProcessKill(CObject* target, CObject* killer)
{
    if (killer == nullptr) return;
    if (killer->GetTeam() == 0) return;
    for (auto& rule : m_rulesKill)
    {
        if ((rule->team == killer->GetTeam() || rule->team == 0) &&
            rule->CheckForObject(target))
        {
            AddPoints(killer->GetTeam(), rule->score);
        }
    }
}

void CScoreboard::ProcessEndTake(int team)
{
    for (auto& rule : m_rulesEndTake)
    {
        if (rule->team == team || rule->team == 0)
        {
            AddPoints(team, rule->score);
        }
    }
}

void CScoreboard::AddPoints(int team, int points)
{
    GetLogger()->Info("Team %d earned %d points\n", team, points);
    CRobotMain::GetInstancePointer()->GetDisplayText()->DisplayText(("<<< Team "+boost::lexical_cast<std::string>(team)+" recieved "+boost::lexical_cast<std::string>(points)+" points! >>>").c_str(), Math::Vector(0.0f,0.0f,0.0f), 15.0f, 60.0f, 10.0f, Ui::TT_WARNING);
    m_score[team] += points;
}

int CScoreboard::GetScore(int team)
{
    return m_score[team];
}