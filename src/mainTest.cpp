
#include <config.h>

#include "mainwindow.h"
#include "translators/translators.h" // needed for file type enum

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include "fetch/fetcherinitializer.h"

int main(int argc, char* argv[]) {
  Tellico::Fetch::FetcherInitializer init;

}
