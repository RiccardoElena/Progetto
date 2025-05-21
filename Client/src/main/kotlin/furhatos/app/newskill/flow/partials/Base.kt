package furhatos.app.newskill.flow.partials

import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.partialState

val Base =
    partialState {

        include(BaseWOCatchAll)
        onResponse {
            // Catches everything else
            furhat.ask("Scusa, non ho capito. Puoi ripetere?")
        }
    }
