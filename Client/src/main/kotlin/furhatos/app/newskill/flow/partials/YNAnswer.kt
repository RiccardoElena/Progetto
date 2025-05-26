package furhatos.app.newskill.flow.partials

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.flow.utils.changeOutputLanguage
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
            changeOutputLanguage(it.language)
            furhat.say(Localization.getLocalizedString("take_your_time", it.language))

            raise(call(Listening) as Response<*>)
        }
        include(ChangeLocale)
        onResponse(cond = { it.intent !is Yes && it.intent !is No }) {
            changeOutputLanguage(it.language)
            furhat.ask(Localization.getLocalizedString("not_understood", it.language))
        }
        include(BaseWOCatchAll)
    }

val Listening =
    state {
        onEntry {
            furhat.listen(5000)
        }

        onResponse {
            changeOutputLanguage(it.language)
            terminate(it)
        }

        onNoResponse {
            furhat.ask(Localization.getLocalizedString("are_you_there"))
        }
    }
