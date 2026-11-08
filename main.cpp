/* =========================================================================
 Luisa Maria Acevedo Agudelo
 Sara Valenthyna Echeverry Panesso 
   =========================================================================
   
   PRACTICE - STARTING CODE (DO NOT CHANGE THE GIVEN SECTIONS)
   Integrated Itinerary and Fare Engine - Medellin Metro System
   Programming Languages and Paradigms (ST0244) - Universidad EAFIT

   Build: g++ -std=c++17 -Wall -Wextra -o practice main.cpp
   Target environment: OnlineGDB, language selector set to C++17

   WHAT YOU RECEIVE
     - the currency formatter, the Leg and ChargedLeg records and the whole
       main() test harness, so that the expected output is reproducible
     - the declarations you must complete, each marked with TODO

   WHAT YOU MUST WRITE
     - PART 1: the TransportMode hierarchy
     - PART 2: the Passenger hierarchy
     - PART 3: chargeItinerary and the Network class
     - PART 4: the four report functions, with STL algorithms only

   The file must compile with ZERO errors and ZERO warnings under
   g++ -std=c++17 -Wall -Wextra. A warning is a defect, not a detail.
   ========================================================================= */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>

/* -------------------------------------------------------------------------
   Utility: Colombian peso formatting (3820 -> "$ 3.820")
   ------------------------------------------------------------------------- */
std::string formatCOP(long long value) {
    std::string digits = std::to_string(value);
    std::string out;
    int count = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        out.push_back(*it);
        if (++count % 3 == 0 && it + 1 != digits.rend()) {
            out.push_back('.');
        }
    }
    std::reverse(out.begin(), out.end());
    return "$ " + out;
}

/* -------------------------------------------------------------------------
   PART 1. Abstract base class of the transport-mode hierarchy
   ------------------------------------------------------------------------- */
class TransportMode {
public:
    TransportMode(std::string name, int stations)
        : name_(std::move(name)), stations_(stations) {}

    virtual ~TransportMode() = default;           // mandatory: deleted through base pointer

    const std::string& name() const { return name_; }
    int stations() const { return stations_; }

// TODO 1.1  Declare baseFare() and serviceType() as PURE VIRTUAL.
    //           This class cannot know either value: that is the whole point.
    
    virtual long long baseFare() const = 0;
    virtual std::string serviceType() const = 0;

// TODO 1.2  Declare the three hooks with a sensible default of true:
    //           allowsStudentFlatFare(), allowsIntegration(),
    //           opensIntegrationWindow().
    
    virtual bool allowsStudentFlatFare() const { return true; }
    virtual bool allowsIntegration() const { return true; }
    virtual bool opensIntegrationWindow() const { return true; }

// TODO 1.3  Implement describe() as a NON-virtual template method that
    //           returns, for example:
    //              "Metro line A (21 stations) - full fare $ 3.820"
    //           It must call the pure virtual members through this object.
    
    std::string describe() const {
        std::string tipo = serviceType();
        std::string stationsText = std::to_string(stations_);
        std::string faretext = formatCOP(baseFare());
        return tipo + " " + name_ + " (" + stationsText + " stations) - full fare " + faretext;
    }

private:
    std::string name_;
    int stations_;
};

// TODO 1.4  Write the five concrete modes. Full fare of the integrated
//           system in 2026: 3820 pesos.
//
//              MetroLine      serviceType "Metro line"    fare 3820

class MetroLine : public TransportMode {
public:
    MetroLine(std::string name, int stations)
        : TransportMode(name, stations) {}

    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Metro line"; }
};
//              TramLine       serviceType "Tram line"     fare 3820
class TramLine : public TransportMode {
public:
    TramLine(std::string name, int stations)
        : TransportMode(name, stations) {}
        
    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Tram line"; }
};
//              CableLine      serviceType "Cable line"    fare 3820
class CableLine : public TransportMode {
public:
    CableLine(std::string name, int stations)
        : TransportMode(name, stations) {}
        
    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Cable line"; }
};

//              TouristCable   inherits from CableLine, adds a surcharge given in its constructor. It must NOT repeat
//                             the 3820 literal: obtain the parent value with
//                             CableLine::baseFare(). No student flat fare,
//                             no integration, does not open a window.


class TouristCable : public CableLine {
public:
    TouristCable(std::string name, int stations, long long recargo)
        : CableLine(name, stations), surcharge_(recargo) {}
        
    long long baseFare() const override {
        return CableLine::baseFare() + surcharge_;
    }
    std::string serviceType() const override { return "Tourist cable"; }
    
    bool allowsStudentFlatFare() const override { return false; }
    bool allowsIntegration() const override { return false; }
    bool opensIntegrationWindow() const override { return false; }

private:
    long long surcharge_;
};

class FeederRoute : public TransportMode {
public:
    FeederRoute(std::string name, int stations)
        : TransportMode(name, stations) {}
        
    std::string serviceType() const override { return "Feeder route"; }
    long long baseFare() const override { return 0; }
    bool opensIntegrationWindow() const override { return false; }
};
//                             
//              FeederRoute    serviceType "Feeder route"  fare 0
//                             Free because the trunk fare covers it, so it
//                             must NOT open an integration window.

/* -------------------------------------------------------------------------
   PART 2. Passenger hierarchy, Non-Virtual Interface idiom
   fareOn() is NOT virtual: it is the template method that clamps the result.
   computeFare() is the customisation point each profile overrides.
   ------------------------------------------------------------------------- */
// TODO 2.1  Write the Passenger hierarchy using the Non-Virtual Interface
//           idiom, which is the C++ way of writing a template method:
//
//              public:    long long fareOn(const TransportMode&) const
//                         NOT virtual. It calls computeFare and clamps the
//                         result with std::max(0LL, ...), because a fare is
//                         never negative.
//              protected: virtual long long computeFare(const TransportMode&) const
//                         Default behaviour: the full fare of the mode.
//
//           Also declare profile() as pure virtual and give the class a
//           virtual destructor.
   class Passenger {
private:
    std::string name_;

protected:
    virtual long long computeFare(const TransportMode& transporte) const {
        return transporte.baseFare();
    }

public:
    Passenger(const std::string& name) : name_(name) {}

    long long fareOn(const TransportMode& transporte) const {
        return std::max(0LL, computeFare(transporte));
    }

    virtual std::string profile() const = 0;
    virtual ~Passenger() = default;
};

//
// TODO 2.2  Write the five profiles. Each one overrides computeFare and
//           obtains the full fare by calling Passenger::computeFare(mode),
//           which is the C++ equivalent of 'super'. Civica fares, 2026:
//
//              FrequentPassenger      "Civica frequent"         3820
//                                     does NOT override computeFare

class FrequentPassenger : public Passenger {
public:
    FrequentPassenger(std::string nombre) : Passenger(nombre) {}

    std::string profile() const override {
        return "Civica frequent";
    }
};

//              StudentPassenger       "Municipal student"       1600 flat,
//                                     but only when the mode allows it

class StudentPassenger : public Passenger {
public:
    StudentPassenger(std::string nombre) : Passenger(nombre) {}

    std::string profile() const override {
        return "Municipal student";
    }

protected:
    long long computeFare(const TransportMode& transporte) const override {
        if (transporte.allowsStudentFlatFare()) {
            return 1600;
        } else {
            return Passenger::computeFare(transporte);
        }
    }
};
// SeniorPassenger "Senior citizen" base - 490
class SeniorPassenger : public Passenger {
public:
    SeniorPassenger(std::string nombre) : Passenger(nombre) {}

    std::string profile() const override {
        return "Senior citizen";
    }

protected:
    long long computeFare(const TransportMode& transporte) const override {
        return Passenger::computeFare(transporte) - 490;
    }
};

class DisabilityPassenger : public Passenger {
public:
    DisabilityPassenger(std::string nombre) : Passenger(nombre) {}

    std::string profile() const override {
        return "Person with disability";
    }

protected:
    long long computeFare(const TransportMode& transporte) const override {
        return Passenger::computeFare(transporte) - 1100;
    }
};

class BearerPassenger : public Passenger {
public:
    BearerPassenger(std::string nombre) : Passenger(nombre) {}

    std::string profile() const override {
        return "Bearer (occasional)";
    }

protected:
    long long computeFare(const TransportMode& transporte) const override {
        return Passenger::computeFare(transporte) + 580;
    }
};
//              DisabilityPassenger    "Person with disability"  base - 1100
//              BearerPassenger        "Bearer (occasional)"     base + 580

/* -------------------------------------------------------------------------
   PART 3. The itinerary and the integration rule
   ------------------------------------------------------------------------- */
struct Leg {
    int minute; // minutes elapsed since the journey started
    const TransportMode* mode; // non-owning observer pointer
};

struct ChargedLeg {
    int minute;
    std::string modeName;
    std::string serviceType;
    long long charge;
    bool integrated;   // true: free transfer inside the window
};

// TODO 3.1  Write the Network class. It OWNS its modes:
//
//              std::vector<std::unique_ptr<TransportMode>> modes_;
//              TransportMode& add(std::unique_ptr<TransportMode> mode);
//              const std::vector<std::unique_ptr<TransportMode>>& modes() const;
//              void printInventory() const;   // one line per mode
//
//           add() returns a reference to the stored object so that main()
//           can keep observing it without owning it.

class Network {
private:
    std::vector<std::unique_ptr<TransportMode>> modes_;

public:
    TransportMode& add(std::unique_ptr<TransportMode> transporte) {
        modes_.push_back(std::move(transporte));
        return *modes_.back();
    }

    const std::vector<std::unique_ptr<TransportMode>>& modes() const {
        return modes_;
    }

    void printInventory() const {
        std::for_each(modes().begin(), modes().end(), [](const std::unique_ptr<TransportMode>& transporte) {
            std::cout << transporte->describe() << "\n";
        });
    }
};

// TODO 3.2  Write chargeItinerary, which applies the integration rule: 
std::vector<ChargedLeg> chargeItinerary(std::vector<Leg> legs, const Passenger& passenger) {

    // ordenar los recorridos por minuto
    std::sort(legs.begin(), legs.end(), [](const Leg& a, const Leg& b) {
        return a.minute < b.minute;
    });

    std::vector<ChargedLeg> results;
    results.reserve(legs.size());

    int transbordos = 0;
    int inicioVentana = 0;
    bool ventanaActiva = false;

    // recorrer los transportes del itinerario
    for (const auto& leg : legs) {

        long long tarifa = passenger.fareOn(*(leg.mode));
        bool integrado = false;

        // comprobar si permite integracion
        if (leg.mode->allowsIntegration()) {

            // comprobar si puede abrir una ventana
            if (leg.mode->opensIntegrationWindow()) {

                bool dentroDeVentana =
                    ventanaActiva &&
                    (leg.minute - inicioVentana <= 90);

                // aplicar hasta dos transbordos gratuitos
                if (dentroDeVentana && transbordos < 2) {
                    tarifa = 0;
                    integrado = true;
                    transbordos++;
                }
                else {
                    // iniciar una nueva ventana
                    inicioVentana = leg.minute;
                    ventanaActiva = true;
                    transbordos = 0;
                }
            }
        }

        // guardar el cobro de cada recorrido
        ChargedLeg result{
            leg.minute,
            leg.mode->name(),
            leg.mode->serviceType(),
            tarifa,
            integrado
        };

        results.push_back(result);
    }

    return results;
}

/* -------------------------------------------------------------------------
   PART 4. Reports built exclusively with STL algorithms and lambdas
   ------------------------------------------------------------------------- */
// TODO 4.1  long long totalPaid(const std::vector<ChargedLeg>&)
//           Total paid. Use std::accumulate with a lambda. No loops.
long long totalPaid(const std::vector<ChargedLeg>& charged) {
    return std::accumulate(charged.begin(), charged.end(), 0LL,
        [](long long sum, const ChargedLeg& c) {
            return sum + c.charge;
        });
}

long long freeTransferCount(const std::vector<ChargedLeg>& charged) {
    return std::count_if(charged.begin(), charged.end(),
        [](const ChargedLeg& c) {
            return c.integrated;
        });
}

std::map<std::string, long long> paidByServiceType(const std::vector<ChargedLeg>& charged) {
    std::map<std::string, long long> result;
    std::for_each(charged.begin(), charged.end(),
        [&result](const ChargedLeg& c) {
            result[c.serviceType] += c.charge;
        });
    return result;
}

std::vector<std::string> namesMatching(
    const Network& network,
    const std::function<bool(const TransportMode&)>& policy)
{
    std::vector<std::string> names;
    const auto& modes = network.modes();
    std::for_each(modes.begin(), modes.end(),
        [&names, &policy](const std::unique_ptr<TransportMode>& modePtr) {
            if (policy(*modePtr)) {
                names.push_back(modePtr->name());
            }
        });
    return names;
}

std::string joined(const std::vector<std::string>& names) {
    if (names.empty()) return "";
    return std::accumulate(std::next(names.begin()), names.end(), names[0],
        [](const std::string& acc, const std::string& name) {
            return acc + ", " + name;
        });
}

/* -------------------------------------------------------------------------
   MAIN
   ------------------------------------------------------------------------- */
int main() {
    std::cout << "============================================================\n";
    std::cout << "    INTEGRATED ITINERARY AND FARE ENGINE - Valle de Aburra\n";
    std::cout << "============================================================\n\n";

    Network network;
    TransportMode& lineA  = network.add(std::make_unique<MetroLine>("A", 21));
    TransportMode& lineB  = network.add(std::make_unique<MetroLine>("B", 7));
    TransportMode& tramTA = network.add(std::make_unique<TramLine>("T-A Ayacucho", 9));
    TransportMode& cableK = network.add(std::make_unique<CableLine>("K Santo Domingo", 4));
    TransportMode& arvi   = network.add(std::make_unique<TouristCable>("L Arvi", 2, 9180));
    TransportMode& feeder = network.add(std::make_unique<FeederRoute>("130 Robledo", 18));

    std::cout << ">> 1. NETWORK INVENTORY (polymorphic describe)\n";
    network.printInventory();
    std::cout << '\n';
    
    /* Itinerary deliberately given out of chronological order, so that the
       sort step is observable. */

    const std::vector<Leg> itinerary = {
        {45,  &cableK},
        {0,   &feeder},
        {12,  &lineA},
        {150, &lineB},
        {70,  &tramTA},
        {200, &arvi}
    };

    const FrequentPassenger frequent("Diego");
    const StudentPassenger  student("Valentina");
    const SeniorPassenger   senior("Dona Amparo");

    std::cout << ">> 2. FULL FARE PER PROFILE ON METRO LINE A\n";
    std::cout << "  " << frequent.profile() << " pays " << formatCOP(frequent.fareOn(lineA)) << '\n';
    std::cout << "  " << student.profile()  << " pays " << formatCOP(student.fareOn(lineA))  << '\n';
    std::cout << "  " << senior.profile()   << " pays " << formatCOP(senior.fareOn(lineA))   << '\n';
    std::cout << "  " << student.profile()  << " on the tourist cable pays "
              << formatCOP(student.fareOn(arvi)) << " (no flat fare there)\n\n";

    const std::vector<ChargedLeg> charged = chargeItinerary(itinerary, frequent);

    std::cout << ">> 3. CHARGED ITINERARY (frequent profile)\n";
    std::for_each(charged.begin(), charged.end(), [](const ChargedLeg& c) {
        std::cout << "  min " << c.minute << " | " << c.serviceType << " " << c.modeName
                  << " | " << formatCOP(c.charge)
                  << (c.integrated ? "  [free transfer]" : "") << '\n';
    });
    std::cout << '\n';

    std::cout << ">> 4. REPORTS BUILT WITH STL ALGORITHMS\n";
    std::cout << "  accumulate: total paid -> " << formatCOP(totalPaid(charged)) << '\n';
    std::cout << "  count_if: free transfers -> " << freeTransferCount(charged) << '\n';

    const auto byType = paidByServiceType(charged);
    std::for_each(byType.begin(), byType.end(),
        [](const std::pair<const std::string, long long>& entry) {
            std::cout << "  map: paid on " << entry.first << " -> "
                      << formatCOP(entry.second) << '\n';
        });

    const auto firstPaid = std::find_if(charged.begin(), charged.end(),
        [](const ChargedLeg& c) { return c.charge > 0; });
    std::cout << "  find_if: first leg that actually paid -> "
              << (firstPaid == charged.end() ? std::string("none") : firstPaid->modeName) << '\n';

    const std::function<bool(const TransportMode&)> bigModes =
        [](const TransportMode& m) { return m.stations() > 8; };
    std::cout << "  injected policy: modes with more than 8 stations -> "
              << joined(namesMatching(network, bigModes)) << '\n';

    std::cout << "  total for the student on the same itinerary -> "
              << formatCOP(totalPaid(chargeItinerary(itinerary, student))) << '\n';
    std::cout << "  total for the senior on the same itinerary -> "
              << formatCOP(totalPaid(chargeItinerary(itinerary, senior))) << '\n';

    std::cout << "\n============================================================\n";
    std::cout << "  End of demonstration.\n";
    std::cout << "============================================================\n";
    return 0;
}
