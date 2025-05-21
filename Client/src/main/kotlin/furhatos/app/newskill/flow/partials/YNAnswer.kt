package furhatos.app.newskill.flow.partials

import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onNoResponse
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.partialState
import furhatos.flow.kotlin.raise
import furhatos.flow.kotlin.state
import furhatos.nlu.Response
import furhatos.nlu.common.Maybe
import furhatos.nlu.common.No
import furhatos.nlu.common.Yes

val YNAnswer =
    partialState {

        onResponse<Maybe> {
            furhat.say("Va bene, fa con comodo. Ti aspetto")

            raise(call(Listening) as Response<*>)
        }
        onResponse(cond = { it.intent !is Yes && it.intent !is No }) {
            furhat.say("Scusa, non ho capito. Puoi ripetere?")
        }
        include(BaseWOCatchAll)
    }

val Listening =
    state {
        onEntry {
            furhat.listen(5000)
        }

        onResponse {
            terminate(it)
        }

        onNoResponse {
            furhat.ask("Hey, ci sei ancora?")
        }
    }
