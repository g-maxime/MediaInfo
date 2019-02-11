package net.mediaarea.mediainfo

import androidx.appcompat.app.AppCompatActivity
import android.os.AsyncTask
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

import kotlinx.android.synthetic.main.activity_plugins_store.*
import kotlinx.android.synthetic.main.plugin_list_content.view.*

data class Plugin(val sku: String, val name: String, val desc: String, val price: String, val owned: Boolean) {
    override fun toString(): String {
        return desc
    }
}

class PluginsStoreActivity : AppCompatActivity() {
    var plugins : List<Plugin> = listOf<Plugin>()

    inner class GetItems: AsyncTask<Void, Void, List<Plugin>>() {
        override fun onPreExecute() {
            super.onPreExecute()

            val rootLayout: LinearLayout = findViewById(R.id.plugins_layout)
            var found = false
            for (i: Int in rootLayout.childCount downTo 1) {
                if (rootLayout.getChildAt(i - 1).id == R.id.spinner_layout)
                    found = true
            }

            if (!found)
                View.inflate(this@PluginsStoreActivity, R.layout.spinner_layout, rootLayout)
        }

        override fun onPostExecute(result: List<Plugin>?) {
            super.onPostExecute(result)

            if (result!=null) {
                plugins=result
            }
            setupRecyclerView(plugins_list)

            val rootLayout: LinearLayout = findViewById(R.id.plugins_layout)
            for (i: Int in rootLayout.childCount downTo 1) {
                if (rootLayout.getChildAt(i - 1).id == R.id.spinner_layout)
                    rootLayout.removeViewAt(i - 1)
            }
        }

        override fun doInBackground(vararg arguments: Void): List<Plugin> {
            //Thread.sleep(5000)
            //return listOf<Plugin>(Plugin("a.id", "Dark theme",  "Dark UI theme", "1.79 €",false), Plugin("b.id", "Development support","Dark UI theme and all future plugins and a very very two lines long text","10 €",true))
            return emptyList<Plugin>()
        }
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_plugins_store)

        setSupportActionBar(toolbar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
    }

    override fun onStart() {
        super.onStart()

        GetItems().execute()
    }

    private fun setupRecyclerView(recyclerView: RecyclerView) {
        recyclerView.adapter = ItemRecyclerViewAdapter(this, plugins)
    }

    class ItemRecyclerViewAdapter(private val parentActivity: PluginsStoreActivity,
                                  private val plugins: List<Plugin>) :
            RecyclerView.Adapter<ItemRecyclerViewAdapter.ViewHolder>() {

        private val onClickListener: View.OnClickListener

        init {
            onClickListener = View.OnClickListener {
            }
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
            val view = LayoutInflater.from(parent.context)
                    .inflate(R.layout.plugin_list_content, parent, false)
            return ViewHolder(view)
        }

        override fun onBindViewHolder(holder: ViewHolder, position: Int) {
            val plugin: Plugin = plugins[position]
            holder.name.text = plugin.name
            holder.price.text = plugin.price
            holder.desc.text = plugin.desc
            holder.sku = plugin.sku

            if (plugin.owned) {
                holder.buy_button.setEnabled(false)
                holder.buy_button.text = parentActivity.getString(R.string.purchased_text)
                holder.buy_button.setCompoundDrawablesWithIntrinsicBounds(0, 0, 0, 0)
            }

            with(holder.itemView) {
                tag = plugin
                setOnClickListener(onClickListener)
            }
        }

        override fun getItemCount(): Int = plugins.size

        inner class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
            val name: TextView = view.name_text
            val desc: TextView = view.desc_text
            val price: TextView = view.price_text
            val buy_button: Button = view.buy_button
            var sku: String = ""

            init {
                view.buy_button.setOnClickListener {
                    //TODO buy code here
                }
            }
        }
    }
}
