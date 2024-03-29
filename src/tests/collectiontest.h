/***************************************************************************
    Copyright (C) 2009-2016 Robby Stephenson <robby@periapsis.org>
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

#ifndef COLLECTIONTEST_H
#define COLLECTIONTEST_H

#include <QObject>

#include "../datavectors.h"

class CollectionTest : public QObject {
Q_OBJECT

private Q_SLOTS:
  void initTestCase();
  void cleanupTestCase();

  void testEmpty();
  void testCollection();
  void testFields();
  void testDerived();
  void testValue();
  void testValue_data();
  void testDtd();
  void testDtd_data();
  void testDuplicate();
  void testMergeFields();
  void testFieldsIntersection();
  void testAppendCollection();
  void testMergeCollection();
  void testBookMatch();
  void testMergeBenchmark();
  void testMatchScore();
  void testMatchScore_data();
  void testGamePlatform();
  void testNonTitle();

private:
  Tellico::Data::CollPtr m_coll;
  Tellico::Data::EntryPtr m_entry;
};

#endif
