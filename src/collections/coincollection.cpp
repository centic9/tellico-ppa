/***************************************************************************
    Copyright (C) 2003-2009 Robby Stephenson <robby@periapsis.org>
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

#include "coincollection.h"

#include <KLocalizedString>

namespace {
  static const char* coin_general = I18N_NOOP("General");
  static const char* coin_personal = I18N_NOOP("Personal");
}

using Tellico::Data::CoinCollection;

CoinCollection::CoinCollection(bool addDefaultFields_, const QString& title_)
   : Collection(title_.isEmpty() ? i18n("My Coins") : title_) {
  setDefaultGroupField(QLatin1String("denomination"));
  if(addDefaultFields_) {
    addFields(defaultFields());
  }
}

Tellico::Data::FieldList CoinCollection::defaultFields() {
  FieldList list;
  FieldPtr field;

  field = createDefaultField(TitleField);
  field->setProperty(QLatin1String("template"), QLatin1String("%{year}%{mintmark} %{type} %{denomination}"));
  field->setFlags(Field::NoDelete | Field::Derived);
  field->setFormatType(FieldFormat::FormatNone);
  list.append(field);

  field = new Field(QLatin1String("type"), i18n("Type"));
  field->setCategory(i18n(coin_general));
  field->setFlags(Field::AllowCompletion | Field::AllowGrouped);
  field->setFormatType(FieldFormat::FormatTitle);
  list.append(field);

  /* TRANSLATORS: denomination refers to the monetary value. */
  field = new Field(QLatin1String("denomination"), i18nc("monetary denomination", "Denomination"));
  field->setCategory(i18n(coin_general));
  field->setFlags(Field::AllowCompletion | Field::AllowGrouped);
  list.append(field);

  field = new Field(QLatin1String("year"), i18n("Year"), Field::Number);
  field->setCategory(i18n(coin_general));
  field->setFlags(Field::AllowMultiple | Field::AllowGrouped);
  list.append(field);

  field = new Field(QLatin1String("mintmark"), i18n("Mint Mark"));
  field->setCategory(i18n(coin_general));
  field->setFlags(Field::AllowGrouped);
  field->setFormatType(FieldFormat::FormatPlain);
  list.append(field);

  field = new Field(QLatin1String("country"), i18n("Country"));
  field->setCategory(i18n(coin_general));
  field->setFlags(Field::AllowCompletion | Field::AllowGrouped);
  field->setFormatType(FieldFormat::FormatPlain);
  list.append(field);

  field = new Field(QLatin1String("currency"), i18n("Currency"));
  field->setCategory(i18n(coin_general));
  field->setFlags(Field::AllowCompletion | Field::AllowGrouped);
  field->setFormatType(FieldFormat::FormatPlain);
  list.append(field);

  field = new Field(QLatin1String("set"), i18n("Coin Set"), Field::Bool);
  field->setCategory(i18n(coin_general));
  list.append(field);

  QStringList grade = i18nc("Coin grade levels - "
                            "Proof-65,Proof-60,Mint State-65,Mint State-60,"
                            "Almost Uncirculated-55,Almost Uncirculated-50,"
                            "Extremely Fine-40,Very Fine-30,Very Fine-20,Fine-12,"
                            "Very Good-8,Good-4,Fair",
                            "Proof-65,Proof-60,Mint State-65,Mint State-60,"
                            "Almost Uncirculated-55,Almost Uncirculated-50,"
                            "Extremely Fine-40,Very Fine-30,Very Fine-20,Fine-12,"
                            "Very Good-8,Good-4,Fair")
                      .split(QRegExp(QLatin1String("\\s*,\\s*")), QString::SkipEmptyParts);
  field = new Field(QLatin1String("grade"), i18n("Grade"), grade);
  field->setCategory(i18n(coin_general));
  field->setFlags(Field::AllowGrouped);
  list.append(field);

  QStringList service = i18nc("Coin grading services - "
                              "PCGS,NGC,ANACS,ICG,ASA,PCI",
                              "PCGS,NGC,ANACS,ICG,ASA,PCI")
                        .split(QRegExp(QLatin1String("\\s*,\\s*")), QString::SkipEmptyParts);
  field = new Field(QLatin1String("service"), i18n("Grading Service"), service);
  field->setCategory(i18n(coin_general));
  field->setFlags(Field::AllowGrouped);
  list.append(field);

  field = new Field(QLatin1String("pur_date"), i18n("Purchase Date"));
  field->setCategory(i18n(coin_personal));
  field->setFormatType(FieldFormat::FormatDate);
  list.append(field);

  field = new Field(QLatin1String("pur_price"), i18n("Purchase Price"));
  field->setCategory(i18n(coin_personal));
  list.append(field);

  field = new Field(QLatin1String("location"), i18n("Location"));
  field->setCategory(i18n(coin_personal));
  field->setFlags(Field::AllowCompletion | Field::AllowGrouped);
  field->setFormatType(FieldFormat::FormatPlain);
  list.append(field);

  field = new Field(QLatin1String("gift"), i18n("Gift"), Field::Bool);
  field->setCategory(i18n(coin_personal));
  list.append(field);

  field = new Field(QLatin1String("obverse"), i18n("Obverse"), Field::Image);
  list.append(field);

  field = new Field(QLatin1String("reverse"), i18n("Reverse"), Field::Image);
  list.append(field);

  field = new Field(QLatin1String("comments"), i18n("Comments"), Field::Para);
  field->setCategory(i18n(coin_personal));
  list.append(field);

  list.append(createDefaultField(IDField));
  list.append(createDefaultField(CreatedDateField));
  list.append(createDefaultField(ModifiedDateField));

  return list;
}

