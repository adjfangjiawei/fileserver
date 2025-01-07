
#pragma once
#include <Utils/IncludeRequire/GlobalRequire.h>
class Flight final : public Orm::Tiny::Model<Flight>, public Orm::Tiny::SoftDeletes<Flight> {
    friend Model;

    using Model::Model;

  private:
    QString u_table{"flights"};
    QString u_primaryKey{"id"};
};

class FlightAA final : public Orm::Tiny::Model<FlightAA>, public Orm::Tiny::SoftDeletes<FlightAA> {
    friend Model;

    using Model::Model;

  private:
    QString u_table{"flightsaa"};
    QString u_primaryKey{"id"};
};
