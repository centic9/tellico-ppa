/***************************************************************************
    Copyright (C) 2011 Robby Stephenson <robby@periapsis.org>
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

#undef QT_NO_CAST_FROM_ASCII

#include "darkhorsefetchertest.h"

#include "../fetch/execexternalfetcher.h"
#include "../entry.h"
#include "../collections/comicbookcollection.h"
#include "../collectionfactory.h"
#include "../images/imagefactory.h"
#include "../images/image.h"
#include "../fieldformat.h"

#include <KConfig>
#include <KConfigGroup>

#include <QTest>
#include <QStandardPaths>

QTEST_GUILESS_MAIN( DarkHorseFetcherTest )

DarkHorseFetcherTest::DarkHorseFetcherTest() : AbstractFetcherTest() {
}

void DarkHorseFetcherTest::initTestCase() {
  const QString python = QStandardPaths::findExecutable(QStringLiteral("python"));
  if(python.isEmpty()) {
    QSKIP("This test requires python", SkipAll);
  }

  Tellico::ImageFactory::init();
  Tellico::RegisterCollection<Tellico::Data::ComicBookCollection> registerComic(Tellico::Data::Collection::ComicBook, "comicbook");
}

void DarkHorseFetcherTest::testComic() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::ComicBook, Tellico::Fetch::Title,
                                       QStringLiteral("axe cop: bad guy earth #1"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::ExecExternalFetcher(this));

  KConfig config(QFINDTESTDATA("../fetch/scripts/dark_horse_comics.py.spec"), KConfig::SimpleConfig);
  KConfigGroup cg = config.group(QStringLiteral("<default>"));
  cg.writeEntry("ExecPath", QFINDTESTDATA("../fetch/scripts/dark_horse_comics.py"));
  // don't sync() and save the new path
  cg.markAsClean();
  fetcher->readConfig(cg, cg.name());

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QCOMPARE(results.size(), 1);
  // the first entry had better be the right one
  Tellico::Data::EntryPtr entry = results.at(0);

  QCOMPARE(entry->field("title"), QStringLiteral("Axe Cop: Bad Guy Earth #1"));
  QCOMPARE(entry->field("pub_year"), QStringLiteral("2011"));
  QCOMPARE(entry->field("genre"), QStringLiteral("Humor"));
  QCOMPARE(entry->field("pages"), QStringLiteral("32"));
  QCOMPARE(entry->field("publisher"), QStringLiteral("Dark Horse Comics"));
  QCOMPARE(entry->field("writer"), QStringLiteral("Malachai Nicolle"));
  QCOMPARE(entry->field("artist"), QStringLiteral("Ethan Nicolle"));
  QVERIFY(!entry->field("comments").isEmpty());
  QVERIFY(!entry->field("cover").isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).contains(QLatin1Char('/')));
  QVERIFY(!Tellico::ImageFactory::imageById(entry->field("cover")).isNull());
}
