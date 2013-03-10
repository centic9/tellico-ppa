/***************************************************************************
    Copyright (C) 2001-2009 Robby Stephenson <robby@periapsis.org>
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

#include <config.h>

#include "mainwindow.h"
#include "translators/translators.h" // needed for file type enum

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>


#include "fetch/fetcherinitializer.h"
#include "fetch/isbndbfetcher.h"
#include "fetch/fetcherjob.h"
#include "entryupdater.h"
#include "entry.h"
#include "entrycomparison.h"
#include "collection.h"
#include "tellico_kernel.h"
#include "progressmanager.h"
#include "gui/statusbar.h"
#include "document.h"
#include "fetch/fetchresult.h"
#include "entrymatchdialog.h"
#include "tellico_debug.h"
#include "datavectors.h"
#include "fetch/fetchmanager.h"

#include <QPair>

#include <klocale.h>
#include <kiconloader.h>
#include <kapplication.h>

/*using Tellico::EntryUpdater;

  typedef QPair<Tellico::Fetch::FetchResult*, bool> UpdateResult;
  typedef QList<UpdateResult> ResultList;

  Tellico::Data::CollPtr m_coll;
  Tellico::Data::EntryList m_entriesToUpdate;
  Tellico::Data::EntryList m_fetchedEntries;
  Tellico::Data::EntryList m_matchedEntries;
  Tellico::Fetch::FetcherVec m_fetchers;
  int m_fetchIndex;
  int m_origEntryCount;
  ResultList m_results;
  bool m_cancelled;

void init();
*/
/*Tellico::Data::EntryList doFetch(Tellico::Fetch::Fetcher::Ptr fetcher,
                                                      const Tellico::Fetch::FetchRequest& request,
                                                      int maxResults) {
  // don't use 'this' as job parent, it crashes
  Tellico::Fetch::FetcherJob* job = new Tellico::Fetch::FetcherJob(0, fetcher, request);
  connect(job, SIGNAL(result(KJob*)), SLOT(slotResult(KJob*)));

  if(maxResults > 0) {
    job->setMaximumResults(maxResults);
  }

  job->start();
  m_loop.exec();
  return m_results;
}*/

//  void mergeCurrent(Data::EntryPtr entry, bool overwrite);


int main(int argc, char* argv[]) {
  KAboutData aboutData("tellico", 0, ki18n("Tellico"),
                       TELLICO_VERSION, ki18n("Tellico - a collection manager for KDE"),
                       KAboutData::License_GPL_V2,
                       ki18n("(c) 2001-2012, Robby Stephenson"), KLocalizedString(),
                       "http://tellico-project.org", "tellico-users@kde.org");
  aboutData.addAuthor(ki18n("Robby Stephenson"), KLocalizedString(), "robby@periapsis.org");
  aboutData.addAuthor(ki18n("Mathias Monnerville"), ki18n("Data source scripts"));
  aboutData.addAuthor(ki18n("Regis Boudin"), KLocalizedString(), "regis@boudin.name");
  aboutData.addAuthor(ki18n("Petri Damstén"), KLocalizedString(), "damu@iki.fi");
  aboutData.addAuthor(ki18n("Sebastian Held"), KLocalizedString());

  aboutData.addCredit(ki18n("Virginie Quesnay"), ki18n("Icons"));
  aboutData.addCredit(ki18n("Amarok"), ki18n("Code examples and general inspiration"),
                      QByteArray(), "http://amarok.kde.org");
  aboutData.addCredit(ki18n("Greg Ward"), ki18n("Author of btparse library"));
  aboutData.addCredit(ki18n("Robert Gamble"), ki18n("Author of libcsv library"));
  aboutData.addCredit(ki18n("Valentin Lavrinenko"), ki18n("Author of rtf2html library"));

  aboutData.addLicense(KAboutData::License_GPL_V3);

  KCmdLineOptions options;
  options.add("nofile", ki18n("Do not reopen the last open file"));
  options.add("bibtex", ki18n("Import <filename> as a bibtex file")); // krazy:exclude=i18ncheckarg
  options.add("mods", ki18n("Import <filename> as a MODS file")); // krazy:exclude=i18ncheckarg
  options.add("ris", ki18n("Import <filename> as a RIS file")); // krazy:exclude=i18ncheckarg
  options.add("+[filename]", ki18n("File to open"));

  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;

  if(app.isSessionRestored()) {
    RESTORE(Tellico::MainWindow);
  } else {
    Tellico::MainWindow* tellico = new Tellico::MainWindow();
    tellico->show();
    tellico->slotShowTipOfDay(false);

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    if(args->count() > 0) {
      if(args->isSet("bibtex")) {
        tellico->importFile(Tellico::Import::Bibtex, args->url(0), Tellico::Import::Replace);
      } else if(args->isSet("mods")) {
        tellico->importFile(Tellico::Import::MODS, args->url(0), Tellico::Import::Replace);
      } else if(args->isSet("ris")) {
        tellico->importFile(Tellico::Import::RIS, args->url(0), Tellico::Import::Replace);
      } else {
        tellico->slotFileOpen(args->url(0));
      }
    } else {
      // bit of a hack, I just want the --nofile option
      // if --nofile is NOT passed, then the file option is set
      // is it's set, then go ahead and check for opening previous file
      tellico->initFileOpen(!args->isSet("file"));
    }
    args->clear(); // some memory savings
  }

  return app.exec();
  /*
  Tellico::Fetch::FetcherInitializer init2;

  
  //Tellico::Fetch::Manager::self()
  
  /*Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Book, Tellico::Fetch::ISBN,
                                       QLatin1String("0789312239"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::ISBNdbFetcher(NULL));

  Tellico::Data::EntryList results = doFetch(fetcher, request);
  printf("Had %d results", results.size());
  */

/*
  Tellico::Data::EntryList entries;
  Tellico::Data::CollPtr coll(new Tellico::Data::Collection(true)); // add default fields
  
  Tellico::Data::FieldPtr aField(new Tellico::Data::Field(QLatin1String("author"),
                                                          QLatin1String("Author")));
  aField->setFlags(Tellico::Data::Field::AllowMultiple);
  aField->setFormatType(Tellico::FieldFormat::FormatName);
  coll->addField(aField);

  Tellico::Data::EntryPtr entry(new Tellico::Data::Entry(coll));
  entry->setField(QLatin1String("author"), QLatin1String("Albert Einstein; Niels Bohr"));
  coll->addEntries(entry);

  Tellico::Data::FieldPtr field(new Tellico::Data::Field(QLatin1String("test"), QLatin1String("Test")));
  field->setProperty(QLatin1String("template"), QLatin1String("%{author}"));
  field->setFlags(Tellico::Data::Field::Derived);
  field->setFormatType(Tellico::FieldFormat::FormatName);
  coll->addField(field);
  
  // it deletes itself when done
  //new EntryUpdater(coll, entries, new QObject());
  
  m_coll = coll;

  // for now, we're assuming all entries are same collection type
  m_fetchers = Tellico::Fetch::Manager::self()->createUpdateFetchers(m_coll->type());
  /*foreach(Fetch::Fetcher::Ptr fetcher, m_fetchers) {
    connect(fetcher.data(), SIGNAL(signalResultFound(Tellico::Fetch::FetchResult*)),
            SLOT(slotResult(Tellico::Fetch::FetchResult*)));
    connect(fetcher.data(), SIGNAL(signalDone(Tellico::Fetch::Fetcher*)),
            SLOT(slotDone()));
  }*/
/*

  init();
  
  
  
  foreach(const UpdateResult& res, m_results) {
    delete res.first;
  }
  m_results.clear();
  

  // clean up
  //delete Tellico::Fetch::Manager::self();
  //KGlobal::deref();
  
    kapp->processEvents();

  return 0;
}

void slotStartNext() {
  //StatusBar::self()->setStatus(i18n("Updating <b>%1</b>...", m_entriesToUpdate.front()->title()));
  //ProgressManager::self()->setProgress(this, m_fetchers.count() * (m_origEntryCount - m_entriesToUpdate.count()) + m_fetchIndex);

  Tellico::Fetch::Fetcher::Ptr f = m_fetchers[m_fetchIndex];
//  myDebug() << "starting " << f->source();
  //f->startUpdate(m_entriesToUpdate.front());
}


void init() {
  m_fetchIndex = 0;
  m_origEntryCount = m_entriesToUpdate.count();
  /*QString label;
  if(m_entriesToUpdate.count() == 1) {
    label = i18n("Updating %1...", m_entriesToUpdate.front()->title());
  } else {
    label = i18n("Updating entries...");
  }
  Kernel::self()->beginCommandGroup(i18n("Update Entries"));
  ProgressItem& item = ProgressManager::self()->newProgressItem(this, label, true /*canCancel*/ /*);
  item.setTotalSteps(m_fetchers.count() * m_origEntryCount);
  connect(&item, SIGNAL(signalCancelled(ProgressItem*)), SLOT(slotCancel()));*/
/*
  // done if no fetchers available
  if(m_fetchers.isEmpty()) {
    //QTimer::singleShot(500, this, SLOT(slotCleanup()));
    myLog() << "No fetchers!";
  } else {
    slotStartNext(); // starts fetching
  }
}

void handleResults() {
  Tellico::Data::EntryPtr entry = m_entriesToUpdate.front();
  int best = 0;
  ResultList matches;
  foreach(const UpdateResult& res, m_results) {
    Tellico::Data::EntryPtr e = res.first->fetchEntry();
    if(!e) {
      continue;
    }
    m_fetchedEntries.append(e);
    int match = m_coll->sameEntry(entry, e);
    if(match) {
//      myDebug() << e->title() << "matches by" << match;
    }
    // if the match is GOOD but not PERFECT, keep all of them
    if(match >= Tellico::EntryComparison::ENTRY_PERFECT_MATCH) {
      if(match > best) {
        best = match;
        matches.clear();
        matches.append(res);
      } else if(match == best) {
        matches.append(res);
      }
    } else if(match >= Tellico::EntryComparison::ENTRY_GOOD_MATCH) {
      best = qMax(best, match);
      // keep all the results that don't exceed the perfect match
      matches.append(res);
    } else if(match > best) {
      best = match;
      matches.clear();
      matches.append(res);
    }
  }
  if(best < Tellico::EntryComparison::ENTRY_GOOD_MATCH) {
    if(best > 0) {
      myDebug() << "no good match (score > 10), best match =" << best << "(" << matches.count() << "matches)";
    }
    return;
  }
//  myDebug() << "best match = " << best << " (" << matches.count() << " matches)";
  UpdateResult match(0, true);
  if(matches.count() == 1) {
    match = matches.front();
  } else if(matches.count() > 1) {
    //match = askUser(matches);
    myLog() << "ask user!";
  }
  // askUser() could come back with nil
  if(match.first) {
    //mergeCurrent(match.first->fetchEntry(), match.second);
    myLog() << "merge";
  }
}

void slotDone() {
  if(m_cancelled) {
    myLog() << "cancelled";
    //QTimer::singleShot(500, this, SLOT(slotCleanup()));
    return;
  }

  if(!m_results.isEmpty()) {
    handleResults();
  }

  m_results.clear();
  ++m_fetchIndex;
//  myDebug() << m_fetchIndex;
  if(m_fetchIndex == m_fetchers.count()) {
    m_fetchIndex = 0;
    // we've gone through the loop for the first entry in the vector
    // pop it and move on
    m_entriesToUpdate.removeAll(m_entriesToUpdate.front());
    // if there are no more entries, and this is the last fetcher, time to delete
    if(m_entriesToUpdate.isEmpty()) {
      //QTimer::singleShot(500, this, SLOT(slotCleanup()));
      return;
    }
  }
  kapp->processEvents();
  // so the entry updater can clean up a bit
  //QTimer::singleShot(500, this, SLOT(slotStartNext()));*/
}

