/***************************************************************************
    Copyright (C) 2007-2009 Robby Stephenson <robby@periapsis.org>
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

#ifndef TELLICO_FETCH_ARXIVFETCHER_H
#define TELLICO_FETCH_ARXIVFETCHER_H

#include "fetcher.h"
#include "configwidget.h"

#include <QPointer>

class QUrl;
class KJob;
namespace KIO {
  class StoredTransferJob;
}

namespace Tellico {

  class XSLTHandler;

  namespace Fetch {

/**
 * @author Robby Stephenson
 */
class ArxivFetcher : public Fetcher {
Q_OBJECT

public:
  ArxivFetcher(QObject* parent);
  ~ArxivFetcher();

  virtual QString source() const override;
  virtual QString attribution() const override;
  virtual bool isSearching() const override { return m_started; }
  virtual void continueSearch() override;
  virtual bool canSearch(FetchKey k) const override;
  virtual void stop() override;
  virtual Data::EntryPtr fetchEntryHook(uint uid) override;
  virtual Type type() const override { return Arxiv; }
  virtual bool canFetch(int type) const override;
  virtual void readConfigHook(const KConfigGroup& config) override;

  virtual Fetch::ConfigWidget* configWidget(QWidget* parent) const override;

  class ConfigWidget : public Fetch::ConfigWidget {
  public:
    explicit ConfigWidget(QWidget* parent_, const ArxivFetcher* fetcher = nullptr);
    virtual void saveConfigHook(KConfigGroup& config) override;
    virtual QString preferredName() const override;
  };
  friend class ConfigWidget;

  static QString defaultName();
  static QString defaultIcon();
  static StringHash allOptionalFields() { return StringHash(); }

private Q_SLOTS:
  void slotComplete(KJob* job);

private:
  virtual void search() override;
  virtual FetchRequest updateRequest(Data::EntryPtr entry) override;
  void initXSLTHandler();
  QUrl searchURL(FetchKey key, const QString& value) const;
  void doSearch();

  XSLTHandler* m_xsltHandler;
  int m_start;
  int m_total;

  QHash<uint, Data::EntryPtr> m_entries;
  QPointer<KIO::StoredTransferJob> m_job;

  bool m_started;
};

  }
}
#endif
