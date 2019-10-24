/***************************************************************************
    Copyright (C) 2009 Robby Stephenson <robby@periapsis.org>
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

#include "openlibraryfetcher.h"
#include "../collections/bookcollection.h"
#include "../images/imagefactory.h"
#include "../utils/isbnvalidator.h"
#include "../utils/guiproxy.h"
#include "../utils/string_utils.h"
#include "../entry.h"
#include "../core/filehandler.h"
#include "../tellico_debug.h"

#include <KLocalizedString>
#include <KIO/Job>
#include <KJobUiDelegate>
#include <KJobWidgets/KJobWidgets>

#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QGridLayout>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>

namespace {
  static const char* OPENLIBRARY_QUERY_URL = "http://openlibrary.org/query.json";
}

using namespace Tellico;
using Tellico::Fetch::OpenLibraryFetcher;

OpenLibraryFetcher::OpenLibraryFetcher(QObject* parent_)
    : Fetcher(parent_), m_started(false) {
}

OpenLibraryFetcher::~OpenLibraryFetcher() {
}

QString OpenLibraryFetcher::source() const {
  return m_name.isEmpty() ? defaultName() : m_name;
}

bool OpenLibraryFetcher::canSearch(FetchKey k) const {
  return k == Title || k == Person || k == ISBN || k == LCCN || k == Keyword;
}

bool OpenLibraryFetcher::canFetch(int type) const {
  return type == Data::Collection::Book || type == Data::Collection::Bibtex;
}

void OpenLibraryFetcher::readConfigHook(const KConfigGroup&) {
}

void OpenLibraryFetcher::search() {
  m_started = true;
  // we only split ISBN and LCCN values
  QStringList searchTerms;
  if(request().key == ISBN || request().key == LCCN) {
    searchTerms = FieldFormat::splitValue(request().value);
  } else  {
    searchTerms += request().value;
  }
  foreach(const QString& searchTerm, searchTerms) {
    doSearch(searchTerm);
  }
  if(m_jobs.isEmpty()) {
    stop();
  }
}

void OpenLibraryFetcher::doSearch(const QString& term_) {
  QUrl u(QString::fromLatin1(OPENLIBRARY_QUERY_URL));
  QUrlQuery q;
  // books are type/edition
  q.addQueryItem(QStringLiteral("type"), QStringLiteral("/type/edition"));
  q.addQueryItem(QStringLiteral("*"), QString());

  switch(request().key) {
    case Title:
      q.addQueryItem(QStringLiteral("title"), term_);
      break;

    case Person:
      {
        const QString author = getAuthorKeys(term_);
        if(author.isEmpty()) {
          myWarning() << "no authors found";
          return;
        }
        q.addQueryItem(QStringLiteral("authors"), author);
      }
      break;

    case ISBN:
      {
        const QString isbn = ISBNValidator::cleanValue(term_);
        if(isbn.size() > 10) {
          q.addQueryItem(QStringLiteral("isbn_13"), isbn);
        } else {
          q.addQueryItem(QStringLiteral("isbn_10"), isbn);
        }
      }
      break;

    case LCCN:
      q.addQueryItem(QStringLiteral("lccn"), term_);
      break;

    case Keyword:
      myWarning() << "not supported";
      return;

    default:
      myWarning() << "key not recognized:" << request().key;
      return;
  }
  u.setQuery(q);
//  myDebug() << "url:" << u;

  QPointer<KIO::StoredTransferJob> job = KIO::storedGet(u, KIO::NoReload, KIO::HideProgressInfo);
  KJobWidgets::setWindow(job, GUI::Proxy::widget());
  connect(job.data(), &KJob::result, this, &OpenLibraryFetcher::slotComplete);
  m_jobs << job;
}

void OpenLibraryFetcher::endJob(KIO::StoredTransferJob* job_) {
  m_jobs.removeAll(job_);
  if(m_jobs.isEmpty())  {
    stop();
  }
}

void OpenLibraryFetcher::stop() {
  if(!m_started) {
    return;
  }
  foreach(QPointer<KIO::StoredTransferJob> job, m_jobs) {
    if(job) {
      job->kill();
    }
  }
  m_jobs.clear();
  m_started = false;
  emit signalDone(this);
}

Tellico::Data::EntryPtr OpenLibraryFetcher::fetchEntryHook(uint uid_) {
  Data::EntryPtr entry = m_entries.value(uid_);
  if(!entry) {
    myWarning() << "no entry in dict";
    return Data::EntryPtr();
  }

  // if the entry is not set, go ahead and try to fetch it
  if(entry->field(QStringLiteral("cover")).isEmpty()) {
    const QString isbn = ISBNValidator::cleanValue(entry->field(QStringLiteral("isbn")));
    if(!isbn.isEmpty()) {
      QUrl imageUrl(QStringLiteral("http://covers.openlibrary.org/b/isbn/%1-M.jpg?default=false").arg(isbn));
      const QString id = ImageFactory::addImage(imageUrl, true);
      if(!id.isEmpty()) {
        entry->setField(QStringLiteral("cover"), id);
      }
    }
  }

  return entry;
}

Tellico::Fetch::FetchRequest OpenLibraryFetcher::updateRequest(Data::EntryPtr entry_) {
  const QString isbn = entry_->field(QStringLiteral("isbn"));
  if(!isbn.isEmpty()) {
    return FetchRequest(ISBN, isbn);
  }
  const QString lccn = entry_->field(QStringLiteral("lccn"));
  if(!lccn.isEmpty()) {
    return FetchRequest(LCCN, lccn);
  }
  const QString title = entry_->field(QStringLiteral("title"));
  if(!title.isEmpty()) {
    return FetchRequest(Title, title);
  }
  return FetchRequest();
}

void OpenLibraryFetcher::slotComplete(KJob* job_) {
  KIO::StoredTransferJob* job = static_cast<KIO::StoredTransferJob*>(job_);
//  myDebug();

  if(job->error()) {
    job->uiDelegate()->showErrorMessage();
    endJob(job);
    return;
  }

  QByteArray data = job->data();
  if(data.isEmpty()) {
    myDebug() << "no data";
    endJob(job);
    return;
  }

#if 0
  myWarning() << "Remove debug from openlibraryfetcher.cpp";
  QFile f(QString::fromLatin1("/tmp/test.json"));
  if(f.open(QIODevice::WriteOnly)) {
    QTextStream t(&f);
    t.setCodec("UTF-8");
    t << data;
  }
  f.close();
#endif

  QJsonDocument doc = QJsonDocument::fromJson(data);
  QJsonArray array = doc.array();
  if(array.isEmpty()) {
//    myDebug() << "no results";
    endJob(job);
    return;
  }

  Data::CollPtr coll(new Data::BookCollection(true));
  if(!coll->hasField(QStringLiteral("openlibrary")) && optionalFields().contains(QStringLiteral("openlibrary"))) {
    Data::FieldPtr field(new Data::Field(QStringLiteral("openlibrary"), i18n("OpenLibrary Link"), Data::Field::URL));
    field->setCategory(i18n("General"));
    coll->addField(field);
  }

  for(int i = 0; i < array.count(); i++) {
    // be sure to check that the fetcher has not been stopped
    // crashes can occur if not
    if(!m_started) {
      break;
    }
//    myDebug() << "found result:" << result;
    QVariantMap resultMap = array.at(i).toObject().toVariantMap();

//  myDebug() << resultMap.value(QLatin1String("isbn_10")).toList().at(0);

    Data::EntryPtr entry(new Data::Entry(coll));

    entry->setField(QStringLiteral("title"), mapValue(resultMap, "title"));
    entry->setField(QStringLiteral("subtitle"), mapValue(resultMap, "subtitle"));
    entry->setField(QStringLiteral("pub_year"), mapValue(resultMap, "publish_date"));
    QString isbn = mapValue(resultMap, "isbn_10");
    if(isbn.isEmpty()) {
      isbn = mapValue(resultMap, "isbn_13");
    }
    if(!isbn.isEmpty()) {
      ISBNValidator val(this);
      val.fixup(isbn);
      entry->setField(QStringLiteral("isbn"), isbn);
    }
    entry->setField(QStringLiteral("lccn"), mapValue(resultMap, "lccn"));
    entry->setField(QStringLiteral("genre"), mapValue(resultMap, "genres"));
    entry->setField(QStringLiteral("keyword"), mapValue(resultMap, "subjects"));
    entry->setField(QStringLiteral("edition"), mapValue(resultMap, "edition_name"));
    QString binding = mapValue(resultMap, "physical_format");
    if(binding.toLower() == QLatin1String("hardcover")) {
      binding = QStringLiteral("Hardback");
    } else if(binding.contains(QStringLiteral("paperback"), Qt::CaseInsensitive)) {
      binding = QStringLiteral("Paperback");
    }
    if(!binding.isEmpty()) {
      entry->setField(QStringLiteral("binding"), i18n(binding.toUtf8().constData()));
    }
    entry->setField(QStringLiteral("publisher"), mapValue(resultMap, "publishers"));
    entry->setField(QStringLiteral("series"), mapValue(resultMap, "series"));
    entry->setField(QStringLiteral("pages"), mapValue(resultMap, "number_of_pages"));
    entry->setField(QStringLiteral("comments"), mapValue(resultMap, "notes"));

    if(optionalFields().contains(QStringLiteral("openlibrary"))) {
      entry->setField(QStringLiteral("openlibrary"), QLatin1String("http://openlibrary.org") + mapValue(resultMap, "key"));
    }

    QStringList authors;
    foreach(const QVariant& authorMap, resultMap.value(QLatin1String("authors")).toList()) {
      const QString key = mapValue(authorMap.toMap(), "key");
      if(!key.isEmpty()) {
        QUrl authorUrl(QString::fromLatin1(OPENLIBRARY_QUERY_URL));
        QUrlQuery q;
        q.addQueryItem(QStringLiteral("type"), QStringLiteral("/type/author"));
        q.addQueryItem(QStringLiteral("key"), key);
        q.addQueryItem(QStringLiteral("name"), QString());
        authorUrl.setQuery(q);

        QString output = FileHandler::readTextFile(authorUrl, true /*quiet*/);
        QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
        QJsonArray array = doc.array();
        QVariantMap authorResult = array.isEmpty() ? QVariantMap() : array.at(0).toObject().toVariantMap();
        const QString name = mapValue(authorResult, "name");
        if(!name.isEmpty()) {
          authors << name;
        }
      }
    }
    if(!authors.isEmpty()) {
      entry->setField(QStringLiteral("author"), authors.join(FieldFormat::delimiterString()));
    }

    QStringList langs;
    foreach(const QVariant& langMap, resultMap.value(QLatin1String("languages")).toList()) {
      const QString key = mapValue(langMap.toMap(), "key");
      if(!key.isEmpty()) {
        QUrl langUrl(QString::fromLatin1(OPENLIBRARY_QUERY_URL));
        QUrlQuery q;
        q.addQueryItem(QStringLiteral("type"), QStringLiteral("/type/language"));
        q.addQueryItem(QStringLiteral("key"), key);
        q.addQueryItem(QStringLiteral("name"), QString());
        langUrl.setQuery(q);

        QString output = FileHandler::readTextFile(langUrl, true /*quiet*/, true /*utf8*/);
        QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
        QJsonArray array = doc.array();
        QVariantMap langResult = array.isEmpty() ? QVariantMap() : array.at(0).toObject().toVariantMap();
        const QString name = mapValue(langResult, "name");
        if(!name.isEmpty()) {
          langs << i18n(name.toUtf8().constData());
        }
      }
    }
    if(!langs.isEmpty()) {
      entry->setField(QStringLiteral("language"), langs.join(FieldFormat::delimiterString()));
    }

    FetchResult* r = new FetchResult(Fetcher::Ptr(this), entry);
    m_entries.insert(r->uid, entry);
    emit signalResultFound(r);
  }

//  m_start = m_entries.count();
//  m_hasMoreResults = m_start <= m_total;
  m_hasMoreResults = false; // for now, no continued searches
  endJob(job);
}

QString OpenLibraryFetcher::getAuthorKeys(const QString& term_) {
  QUrl u(QString::fromLatin1(OPENLIBRARY_QUERY_URL));
  QUrlQuery q;
  q.addQueryItem(QStringLiteral("type"), QStringLiteral("/type/author"));
  q.addQueryItem(QStringLiteral("name"), term_);
  u.setQuery(q);

  QString output = FileHandler::readTextFile(u, true /*quiet*/, true /*utf8*/);
  QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
  QJsonArray array = doc.array();
//  myDebug() << "found" << array.count() << "authors";
  // right now, only use the first
  return array.isEmpty() ? QString() : mapValue(array.at(0).toObject().toVariantMap(), "key");
}

Tellico::Fetch::ConfigWidget* OpenLibraryFetcher::configWidget(QWidget* parent_) const {
  return new OpenLibraryFetcher::ConfigWidget(parent_, this);
}

QString OpenLibraryFetcher::defaultName() {
  return QStringLiteral("Open Library"); // no translation
}

QString OpenLibraryFetcher::defaultIcon() {
  return favIcon("http://openlibrary.org");
}

Tellico::StringHash OpenLibraryFetcher::allOptionalFields() {
  StringHash hash;
  hash[QStringLiteral("openlibrary")] = i18n("OpenLibrary Link");
  return hash;
}

OpenLibraryFetcher::ConfigWidget::ConfigWidget(QWidget* parent_, const OpenLibraryFetcher* fetcher_)
    : Fetch::ConfigWidget(parent_) {
  QVBoxLayout* l = new QVBoxLayout(optionsWidget());
  l->addWidget(new QLabel(i18n("This source has no options."), optionsWidget()));
  l->addStretch();

  // now add additional fields widget
  addFieldsWidget(OpenLibraryFetcher::allOptionalFields(), fetcher_ ? fetcher_->optionalFields() : QStringList());
}

void OpenLibraryFetcher::ConfigWidget::saveConfigHook(KConfigGroup&) {
}

QString OpenLibraryFetcher::ConfigWidget::preferredName() const {
  return OpenLibraryFetcher::defaultName();
}
