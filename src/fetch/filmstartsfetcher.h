/***************************************************************************
    Copyright (C) 2012 Robby Stephenson <robby@periapsis.org>
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

#ifndef TELLICO_FILMSTARTSFETCHER_H
#define TELLICO_FILMSTARTSFETCHER_H

#include "allocinefetcher.h"

namespace Tellico {

  namespace Fetch {

/**
 * A fetcher for filmstarts.de
 *
 * @author Robby Stephenson
 */
class FilmStartsFetcher : public AbstractAllocineFetcher {
Q_OBJECT

public:
  /**
   */
  FilmStartsFetcher(QObject* parent);

  virtual QString source() const;
  virtual Type type() const { return FilmStarts; }

  /**
   * Returns a widget for modifying the fetcher's config.
   */
  virtual Fetch::ConfigWidget* configWidget(QWidget* parent) const;

  class ConfigWidget : public AbstractAllocineFetcher::ConfigWidget {
  public:
    explicit ConfigWidget(QWidget* parent_, const AbstractAllocineFetcher* fetcher = 0);
    virtual QString preferredName() const;
  };

  static QString defaultName();
  static QString defaultIcon();
  static StringHash allOptionalFields();
};

  } // end namespace
} // end namespace
#endif
