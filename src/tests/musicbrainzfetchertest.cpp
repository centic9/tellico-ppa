/***************************************************************************
    Copyright (C) 2010-2011 Robby Stephenson <robby@periapsis.org>
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

#include "musicbrainzfetchertest.h"

#include "../fetch/musicbrainzfetcher.h"
#include "../collections/musiccollection.h"
#include "../collectionfactory.h"
#include "../entry.h"
#include "../images/imagefactory.h"
#include "../utils/datafileregistry.h"

#include <QTest>

QTEST_GUILESS_MAIN( MusicBrainzFetcherTest )

MusicBrainzFetcherTest::MusicBrainzFetcherTest() : AbstractFetcherTest() {
}

void MusicBrainzFetcherTest::initTestCase() {
  Tellico::RegisterCollection<Tellico::Data::MusicCollection> registerMusic(Tellico::Data::Collection::Album, "album");
  // since we use the importer
  Tellico::DataFileRegistry::self()->addDataLocation(QFINDTESTDATA("../../xslt/musicbrainz2tellico.xsl"));
  Tellico::ImageFactory::init();

  m_fieldValues.insert(QStringLiteral("title"), QStringLiteral("carried along"));
  m_fieldValues.insert(QStringLiteral("artist"), QStringLiteral("Andrew Peterson"));
  m_fieldValues.insert(QStringLiteral("label"), QStringLiteral("essential records"));
  m_fieldValues.insert(QStringLiteral("year"), QStringLiteral("2000"));
  m_fieldValues.insert(QStringLiteral("medium"), QStringLiteral("compact disc"));
}

void MusicBrainzFetcherTest::testTitle() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Album, Tellico::Fetch::Title,
                                       m_fieldValues.value(QStringLiteral("title")));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::MusicBrainzFetcher(this));

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QCOMPARE(results.size(), 1);

  Tellico::Data::EntryPtr entry = results.at(0);
  QHashIterator<QString, QString> i(m_fieldValues);
  while(i.hasNext()) {
    i.next();
    QString result = entry->field(i.key()).toLower();
    QCOMPARE(result, i.value().toLower());
  }
  QVERIFY(!entry->field(QStringLiteral("track")).isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).contains(QLatin1Char('/')));
}

void MusicBrainzFetcherTest::testKeyword() {
  // the total test case ends up exceeding the throttle limit so pause for a second
  QTest::qWait(1000);

  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Album, Tellico::Fetch::Keyword,
                                       m_fieldValues.value(QStringLiteral("title")));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::MusicBrainzFetcher(this));

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QCOMPARE(results.size(), 1);

  Tellico::Data::EntryPtr entry = results.at(0);
  QHashIterator<QString, QString> i(m_fieldValues);
  while(i.hasNext()) {
    i.next();
    QString result = entry->field(i.key()).toLower();
    QCOMPARE(result, i.value().toLower());
  }
  QVERIFY(!entry->field(QStringLiteral("track")).isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).contains(QLatin1Char('/')));
}

void MusicBrainzFetcherTest::testPerson() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Album, Tellico::Fetch::Person,
                                       m_fieldValues.value(QStringLiteral("artist")));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::MusicBrainzFetcher(this));

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 20);

  QVERIFY(results.size() > 0);
  Tellico::Data::EntryPtr entry;  //  results can be randomly ordered, loop until we find the one we want
  foreach(Tellico::Data::EntryPtr test, results) {
    if(test->field(QStringLiteral("title")).toLower() == m_fieldValues.value(QStringLiteral("title"))) {
      entry = test;
      break;
    } else {
      qDebug() << "skipping" << test->title();
    }
  }
  QVERIFY(entry);

  QHashIterator<QString, QString> i(m_fieldValues);
  while(i.hasNext()) {
    i.next();
    QString result = entry->field(i.key()).toLower();
    QCOMPARE(result, i.value().toLower());
  }
  QVERIFY(!entry->field(QStringLiteral("track")).isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).contains(QLatin1Char('/')));
}

// test grabbing cover art from coverartarchive.org
void MusicBrainzFetcherTest::testCoverArt() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Album, Tellico::Fetch::Title,
                                       QStringLiteral("Laulut ja tarinat"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::MusicBrainzFetcher(this));

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QVERIFY(!results.isEmpty());

  Tellico::Data::EntryPtr entry = results.at(0);
  QCOMPARE(entry->title(), QStringLiteral("Laulut ja tarinat"));
  QVERIFY(!entry->field(QStringLiteral("cover")).isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).contains(QLatin1Char('/')));
}

void MusicBrainzFetcherTest::testSoundtrack() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Album, Tellico::Fetch::Title,
                                       QStringLiteral("legend of bagger vance"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::MusicBrainzFetcher(this));

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QVERIFY(!results.isEmpty());

  Tellico::Data::EntryPtr entry = results.at(0);
  QCOMPARE(entry->title(), QStringLiteral("The Legend of Bagger Vance"));
  // sound tracks are the only genre tag that is read
  QCOMPARE(entry->field(QStringLiteral("genre")), QStringLiteral("Soundtrack"));
}
