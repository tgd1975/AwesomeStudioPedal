import 'dart:async';

import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:provider/provider.dart';
import '../models/profiles_state.dart';
import '../services/community_profiles_service.dart';
import '../theme/asp_theme.dart';

class CommunityProfilesScreen extends StatefulWidget {
  const CommunityProfilesScreen({super.key, CommunityProfilesService? service})
      : _service = service;

  final CommunityProfilesService? _service;

  @override
  State<CommunityProfilesScreen> createState() =>
      _CommunityProfilesScreenState();
}

class _CommunityProfilesScreenState extends State<CommunityProfilesScreen> {
  late final CommunityProfilesService _service =
      widget._service ?? CommunityProfilesService();

  List<ProfileIndexEntry>? _entries;
  String? _error;
  bool _loading = true;

  String _search = '';
  int? _filterButtons;

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    try {
      final entries = await _service.fetchIndex();
      setState(() {
        _entries = entries;
        _loading = false;
      });
    } catch (e) {
      setState(() {
        _error = e.toString();
        _loading = false;
      });
    }
  }

  List<ProfileIndexEntry> get _filtered {
    final q = _search.toLowerCase();
    return (_entries ?? []).where((e) {
      if (_filterButtons != null && e.minButtons > _filterButtons!) {
        return false;
      }
      if (q.isNotEmpty) {
        final haystack = '${e.name} ${e.tags.join(' ')}'.toLowerCase();
        if (!haystack.contains(q)) return false;
      }
      return true;
    }).toList();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Community Profiles')),
      body: Column(
        children: [
          _SearchFilterBar(
            onSearch: (v) => setState(() => _search = v),
            onFilter: (v) => setState(() => _filterButtons = v),
          ),
          Expanded(child: _buildBody(context)),
        ],
      ),
    );
  }

  Widget _buildBody(BuildContext context) {
    if (_loading) {
      return const Center(child: CircularProgressIndicator());
    }
    if (_error != null) {
      return Center(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              Text('Could not load profiles:\n$_error',
                  textAlign: TextAlign.center),
              const SizedBox(height: 16),
              FilledButton(onPressed: _load, child: const Text('Retry')),
            ],
          ),
        ),
      );
    }

    final entries = _filtered;
    if (entries.isEmpty) {
      return const Center(child: Text('No profiles match this filter.'));
    }

    return ListView.separated(
      padding: const EdgeInsets.all(12),
      itemCount: entries.length,
      separatorBuilder: (_, __) => const SizedBox(height: 8),
      itemBuilder: (ctx, i) => _ProfileCard(
        entry: entries[i],
        service: _service,
      ),
    );
  }
}

class _SearchFilterBar extends StatelessWidget {
  const _SearchFilterBar({required this.onSearch, required this.onFilter});

  final ValueChanged<String> onSearch;
  final ValueChanged<int?> onFilter;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.fromLTRB(12, 12, 12, 4),
      child: Row(
        children: [
          Expanded(
            child: TextField(
              decoration: const InputDecoration(
                hintText: 'Search by name or tag…',
                prefixIcon: Icon(Icons.search),
                isDense: true,
                border: OutlineInputBorder(),
              ),
              onChanged: onSearch,
            ),
          ),
          const SizedBox(width: 8),
          DropdownButton<int?>(
            hint: const Text('Buttons'),
            value: null,
            items: const [
              DropdownMenuItem(value: null, child: Text('Any')),
              DropdownMenuItem(value: 1, child: Text('1-btn')),
              DropdownMenuItem(value: 2, child: Text('2-btn')),
              DropdownMenuItem(value: 3, child: Text('3-btn')),
              DropdownMenuItem(value: 4, child: Text('4-btn')),
            ],
            onChanged: onFilter,
          ),
        ],
      ),
    );
  }
}

class _ProfileCard extends StatefulWidget {
  const _ProfileCard({required this.entry, required this.service});

  final ProfileIndexEntry entry;
  final CommunityProfilesService service;

  @override
  State<_ProfileCard> createState() => _ProfileCardState();
}

class _ProfileCardState extends State<_ProfileCard> {
  bool _expanded = false;
  bool _loadingSet = false;

  @override
  Widget build(BuildContext context) {
    final entry = widget.entry;
    final theme = Theme.of(context);

    return Card(
      child: InkWell(
        borderRadius: BorderRadius.circular(12),
        onTap: () => setState(() => _expanded = !_expanded),
        child: Padding(
          padding: const EdgeInsets.all(14),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Container(
                    padding:
                        const EdgeInsets.symmetric(horizontal: 8, vertical: 2),
                    decoration: BoxDecoration(
                      color: theme.colorScheme.primaryContainer,
                      borderRadius: BorderRadius.circular(4),
                    ),
                    child: Text(
                      '${entry.minButtons}-btn',
                      style: TextStyle(
                          fontSize: 11,
                          color: theme.colorScheme.onPrimaryContainer),
                    ),
                  ),
                  const SizedBox(width: 8),
                  Expanded(
                    child: Text(entry.name,
                        style: const TextStyle(
                            fontWeight: FontWeight.w600, fontSize: 15)),
                  ),
                  Icon(_expanded ? Icons.expand_less : Icons.expand_more,
                      color: AspTokens.textMuted),
                ],
              ),
              if (entry.minFirmware != null) ...[
                const SizedBox(height: 6),
                Container(
                  padding:
                      const EdgeInsets.symmetric(horizontal: 8, vertical: 2),
                  decoration: BoxDecoration(
                    color: AspTokens.warning.withValues(alpha: 0.08),
                    borderRadius: BorderRadius.circular(4),
                    border: Border.all(
                        color: AspTokens.warning.withValues(alpha: 0.4)),
                  ),
                  child: Text(
                    'Requires firmware \u2265 ${entry.minFirmware}',
                    style:
                        const TextStyle(fontSize: 11, color: AspTokens.warning),
                  ),
                ),
              ],
              const SizedBox(height: 6),
              Text(entry.description,
                  maxLines: _expanded ? null : 2,
                  overflow:
                      _expanded ? TextOverflow.visible : TextOverflow.ellipsis,
                  style: const TextStyle(
                      fontSize: 13, color: AspTokens.textMuted)),
              const SizedBox(height: 6),
              Wrap(
                spacing: 4,
                runSpacing: 4,
                children: entry.tags
                    .map((t) => Chip(
                          label: Text(t, style: const TextStyle(fontSize: 11)),
                          padding: EdgeInsets.zero,
                          visualDensity: VisualDensity.compact,
                        ))
                    .toList(),
              ),
              const SizedBox(height: 4),
              Text(
                '${entry.profileCount} profile${entry.profileCount != 1 ? 's' : ''}'
                ' \u2022 by ${entry.author}',
                style:
                    const TextStyle(fontSize: 12, color: AspTokens.textMuted),
              ),
              if (_expanded) ...[
                const SizedBox(height: 8),
                const Text('Profiles in this set:',
                    style:
                        TextStyle(fontSize: 12, fontWeight: FontWeight.w600)),
                ...entry.profiles.map((p) => Padding(
                      padding: const EdgeInsets.only(left: 8, top: 2),
                      child: Text('\u2022 $p',
                          style: const TextStyle(fontSize: 12)),
                    )),
                const SizedBox(height: 10),
                Align(
                  alignment: Alignment.centerRight,
                  child: _loadingSet
                      ? const SizedBox(
                          width: 24,
                          height: 24,
                          child: CircularProgressIndicator(strokeWidth: 2))
                      : FilledButton(
                          onPressed: () => _loadSet(context),
                          child: const Text('Load'),
                        ),
                ),
              ],
            ],
          ),
        ),
      ),
    );
  }

  Future<void> _loadSet(BuildContext context) async {
    setState(() => _loadingSet = true);
    try {
      final json = await widget.service.fetchProfileSet(widget.entry.id);
      if (!context.mounted) return;
      context.read<ProfilesState>().loadFromJson(json);
      unawaited(context.push('/profiles'));
    } catch (e) {
      if (!context.mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Could not load profile: $e')),
      );
    } finally {
      if (mounted) setState(() => _loadingSet = false);
    }
  }
}
