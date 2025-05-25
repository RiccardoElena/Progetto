package furhatos.app.newskill.flow.partials

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.flow.utils.changeOutputLanguage
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.partialState

val Base =
    partialState {

        include(BaseWOCatchAll)
        onResponse {
            // Catches everything else
            changeOutputLanguage(it.language)
            furhat.ask(Localization.getLocalizedString("not_understood", it.language))
        }
    }
