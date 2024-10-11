/***************************************************************************
    Copyright (C) 2023 Robby Stephenson <robby@periapsis.org>
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

#include "ebookimporter.h"
#include "filereaderbook.h"
#include "../collections/bookcollection.h"
#include "../tellico_debug.h"

#include <KLocalizedString>
#include <KFileItem>

#include <QPixmap>

#include <memory>

using Tellico::Import::EBookImporter;

EBookImporter::EBookImporter(const QUrl& url_) : Importer(url_) {
}

EBookImporter::EBookImporter(const QList<QUrl>& urls_) : Importer(urls_) {
}

bool EBookImporter::canImport(int type) const {
  return type == Data::Collection::Book;
}

Tellico::Data::CollPtr EBookImporter::collection() {
  const auto urls = this->urls();
  if(urls.isEmpty()) {
    return Data::CollPtr();
  }
  auto reader = std::make_unique<Tellico::FileReaderBook>(urls.first());
  reader->setUseFilePreview(true);

  Data::CollPtr coll(new Data::BookCollection(true));

#ifdef HAVE_KFILEMETADATA
  Data::EntryList entries;
  foreach(const QUrl& url, urls) {
    KFileItem item(url);
//    myDebug() << "Reading" << url.url() << item.mimetype();
    Data::EntryPtr entry(new Data::Entry(coll));
    if(reader->populate(entry, item)) {
      entry->setField(QStringLiteral("comments"), url.toLocalFile());
      entries += entry;
    }
  }
  coll->addEntries(entries);
#endif
  return coll;
}

void EBookImporter::slotCancel() {
  myDebug() << "EBookImporter::slotCancel() - unimplemented";
}
