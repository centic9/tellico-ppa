/***************************************************************************
    Copyright (C) 2005-2009 Robby Stephenson <robby@periapsis.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#ifndef TELLICO_FETCH_MESSAGEHANDLER_H
#define TELLICO_FETCH_MESSAGEHANDLER_H

#include <QString>

class QStringList;

namespace Tellico {
  namespace Fetch {

/**
 * @author Robby Stephenson
 */
class MessageHandler {
public:
  enum Type { Status, Warning, Error, ListError };

  MessageHandler() {}
  virtual ~MessageHandler() {}

  virtual void send(const QString& message, Type type) = 0;
  virtual void infoList(const QString& message, const QStringList& list) = 0;
};

class ManagerMessage : public MessageHandler {
public:
  ManagerMessage() : MessageHandler() {}
  virtual ~ManagerMessage() {}

  virtual void send(const QString& message, Type type) Q_DECL_OVERRIDE;
  virtual void infoList(const QString& message, const QStringList& list) Q_DECL_OVERRIDE;
};

  } // end namespace
} // end namespace

#endif
